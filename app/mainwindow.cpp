#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "querytab.h"

#include <QAction>
#include <QCloseEvent>
#include <QComboBox>
#include <QFutureWatcher>
#include <QKeySequence>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QSettings>
#include <QTabWidget>
#include <QToolButton>
#include <QTreeWidget>
#include <QtConcurrent>

namespace {
constexpr int KindRole = Qt::UserRole;
constexpr int IdRole = Qt::UserRole + 1;
const QString kDataset = QStringLiteral("dataset");
const QString kTable = QStringLiteral("table");
const QString kPlaceholder = QStringLiteral("placeholder");
const QString kLoading = QStringLiteral("loading");
const QString kProjectsKey = QStringLiteral("projects/history");

QTreeWidgetItem *makePlaceholder(QTreeWidgetItem *parent) {
  auto *placeholder = new QTreeWidgetItem(parent);
  placeholder->setData(0, KindRole, kPlaceholder);
  placeholder->setText(0, QStringLiteral("…"));
  return placeholder;
}
} // namespace

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  ui->projectCombo->lineEdit()->setPlaceholderText(tr("GCP project id"));

  ui->tabWidget->setTabsClosable(true);
  ui->tabWidget->setMovable(true);
  auto *newTabButton = new QToolButton(this);
  newTabButton->setText(QStringLiteral("+"));
  newTabButton->setToolTip(tr("New query tab"));
  newTabButton->setAutoRaise(true);
  connect(newTabButton, &QToolButton::clicked, this, &MainWindow::newTab);
  ui->tabWidget->setCornerWidget(newTabButton, Qt::TopRightCorner);
  connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);

  QMenu *queryMenu = menuBar()->addMenu(tr("Query"));
  QAction *runAction = queryMenu->addAction(tr("Run"));
  runAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return));
  connect(runAction, &QAction::triggered, this, [this]() {
    if (QueryTab *tab = currentTab())
      tab->runQuery();
  });
  QAction *newTabAction = queryMenu->addAction(tr("New Tab"));
  newTabAction->setShortcut(QKeySequence::AddTab);
  connect(newTabAction, &QAction::triggered, this, &MainWindow::newTab);
  QAction *closeTabAction = queryMenu->addAction(tr("Close Tab"));
  closeTabAction->setShortcut(QKeySequence::Close);
  connect(closeTabAction, &QAction::triggered, this,
          [this]() { closeTab(ui->tabWidget->currentIndex()); });

  QMenu *viewMenu = menuBar()->addMenu(tr("View"));
  QAction *toggleDock = ui->datasetDock->toggleViewAction();
  toggleDock->setText(tr("Datasets"));
  viewMenu->addAction(toggleDock);

  loadProjectHistory();
  const QString envProject = qEnvironmentVariable("GOOGLE_CLOUD_PROJECT");
  if (!envProject.isEmpty()) {
    if (ui->projectCombo->findText(envProject) < 0)
      ui->projectCombo->insertItem(0, envProject);
    ui->projectCombo->setCurrentText(envProject);
  }

  connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::loadDatasets);
  connect(ui->projectCombo, &QComboBox::textActivated, this, [this](const QString &) { loadDatasets(); });
  connect(ui->projectCombo->lineEdit(), &QLineEdit::returnPressed, this, &MainWindow::loadDatasets);
  connect(ui->datasetTree, &QTreeWidget::itemExpanded, this, &MainWindow::loadTables);
  connect(ui->datasetTree, &QTreeWidget::itemDoubleClicked, this,
          [this](QTreeWidgetItem *item, int) { prefillSelect(item); });

  restoreUi();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  saveUi();
  QMainWindow::closeEvent(event);
}

QueryTab *MainWindow::addTab(const QString &sql)
{
  auto *tab = new QueryTab([this]() { return ui->projectCombo->currentText().trimmed(); });
  if (!sql.isEmpty())
    tab->setQueryText(sql);
  tab->restoreSplitterState(m_splitterState);
  connect(tab, &QueryTab::status, this,
          [this](const QString &message) { statusBar()->showMessage(message); });
  const int index = ui->tabWidget->addTab(tab, tr("Query %1").arg(++m_tabCounter));
  ui->tabWidget->setCurrentIndex(index);
  return tab;
}

QueryTab *MainWindow::currentTab() const
{
  return qobject_cast<QueryTab *>(ui->tabWidget->currentWidget());
}

void MainWindow::newTab()
{
  addTab();
}

void MainWindow::closeTab(int index)
{
  QWidget *widget = ui->tabWidget->widget(index);
  if (!widget)
    return;
  if (auto *tab = qobject_cast<QueryTab *>(widget))
    m_splitterState = tab->splitterState();
  ui->tabWidget->removeTab(index);
  widget->deleteLater();
  if (ui->tabWidget->count() == 0)
    addTab();
}

void MainWindow::loadDatasets()
{
  const QString project = ui->projectCombo->currentText().trimmed();
  if (project.isEmpty()) {
    statusBar()->showMessage(tr("Enter a project to browse datasets."));
    return;
  }
  rememberProject(project);
  ui->datasetTree->clear();
  statusBar()->showMessage(tr("Loading datasets…"));

  auto *watcher = new QFutureWatcher<DatasetListResult>(this);
  connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher, project]() {
    watcher->deleteLater();
    if (project != ui->projectCombo->currentText().trimmed())
      return;
    const DatasetListResult r = watcher->result();
    if (!r.ok) {
      statusBar()->showMessage(tr("Datasets failed: %1").arg(r.error));
      return;
    }
    for (const DatasetInfo &d : r.datasets) {
      auto *item = new QTreeWidgetItem(ui->datasetTree);
      item->setText(0, d.id);
      item->setData(0, KindRole, kDataset);
      item->setData(0, IdRole, d.id);
      const QString tip = QStringLiteral("%1\n%2").arg(d.friendlyName, d.location).trimmed();
      if (!tip.isEmpty())
        item->setToolTip(0, tip);
      makePlaceholder(item);
    }
    statusBar()->showMessage(tr("%n dataset(s)", "", static_cast<int>(r.datasets.size())));
  });
  watcher->setFuture(QtConcurrent::run(bqListDatasets, project));
}

void MainWindow::loadTables(QTreeWidgetItem *datasetItem)
{
  if (!datasetItem || datasetItem->data(0, KindRole).toString() != kDataset)
    return;
  if (datasetItem->childCount() != 1)
    return;
  QTreeWidgetItem *placeholder = datasetItem->child(0);
  if (placeholder->data(0, KindRole).toString() != kPlaceholder)
    return;
  placeholder->setData(0, KindRole, kLoading);
  placeholder->setText(0, tr("Loading…"));

  const QString project = ui->projectCombo->currentText().trimmed();
  const QString datasetId = datasetItem->data(0, IdRole).toString();

  auto *watcher = new QFutureWatcher<TableListResult>(this);
  connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher, project, datasetId]() {
    watcher->deleteLater();
    if (project != ui->projectCombo->currentText().trimmed())
      return;
    QTreeWidgetItem *node = findDatasetItem(datasetId);
    if (!node)
      return;
    const TableListResult r = watcher->result();
    qDeleteAll(node->takeChildren());
    if (!r.ok) {
      statusBar()->showMessage(tr("Tables failed: %1").arg(r.error));
      makePlaceholder(node);
      return;
    }
    for (const TableInfo &t : r.tables) {
      auto *child = new QTreeWidgetItem(node);
      child->setText(0, t.type == QStringLiteral("TABLE")
                            ? t.id
                            : QStringLiteral("%1  [%2]").arg(t.id, t.type));
      child->setData(0, KindRole, kTable);
      child->setData(0, IdRole, t.id);
      child->setToolTip(0, t.type);
    }
    statusBar()->showMessage(tr("%n table(s)", "", static_cast<int>(r.tables.size())));
  });
  watcher->setFuture(QtConcurrent::run(bqListTables, project, datasetId));
}

void MainWindow::prefillSelect(QTreeWidgetItem *tableItem)
{
  if (!tableItem || tableItem->data(0, KindRole).toString() != kTable)
    return;
  QTreeWidgetItem *parent = tableItem->parent();
  if (!parent)
    return;
  const QString project = ui->projectCombo->currentText().trimmed();
  const QString dataset = parent->data(0, IdRole).toString();
  const QString table = tableItem->data(0, IdRole).toString();
  QueryTab *tab = currentTab();
  if (!tab)
    tab = addTab();
  tab->setQueryText(
      QStringLiteral("SELECT *\nFROM `%1.%2.%3`\nLIMIT 1000").arg(project, dataset, table));
}

void MainWindow::restoreUi()
{
  QSettings settings;
  restoreGeometry(settings.value(QStringLiteral("mainwindow/geometry")).toByteArray());
  restoreState(settings.value(QStringLiteral("mainwindow/state")).toByteArray());
  m_splitterState = settings.value(QStringLiteral("ui/splitter")).toByteArray();

  const QStringList queries = settings.value(QStringLiteral("tabs/queries")).toStringList();
  for (const QString &sql : queries)
    addTab(sql);
  if (ui->tabWidget->count() == 0)
    addTab();

  const int current = settings.value(QStringLiteral("tabs/current"), 0).toInt();
  if (current >= 0 && current < ui->tabWidget->count())
    ui->tabWidget->setCurrentIndex(current);
}

void MainWindow::saveUi()
{
  QSettings settings;
  settings.setValue(QStringLiteral("mainwindow/geometry"), saveGeometry());
  settings.setValue(QStringLiteral("mainwindow/state"), saveState());

  if (QueryTab *tab = currentTab())
    m_splitterState = tab->splitterState();
  settings.setValue(QStringLiteral("ui/splitter"), m_splitterState);

  QStringList queries;
  for (int i = 0; i < ui->tabWidget->count(); ++i) {
    if (auto *tab = qobject_cast<QueryTab *>(ui->tabWidget->widget(i)))
      queries << tab->queryText();
  }
  settings.setValue(QStringLiteral("tabs/queries"), queries);
  settings.setValue(QStringLiteral("tabs/current"), ui->tabWidget->currentIndex());
}

void MainWindow::loadProjectHistory()
{
  QSettings settings;
  ui->projectCombo->addItems(settings.value(kProjectsKey).toStringList());
}

void MainWindow::rememberProject(const QString &project)
{
  if (project.isEmpty())
    return;
  QSettings settings;
  QStringList history = settings.value(kProjectsKey).toStringList();
  history.removeAll(project);
  history.prepend(project);
  while (history.size() > 20)
    history.removeLast();
  settings.setValue(kProjectsKey, history);

  if (ui->projectCombo->findText(project) < 0)
    ui->projectCombo->insertItem(0, project);
}

QTreeWidgetItem *MainWindow::findDatasetItem(const QString &datasetId) const
{
  for (int i = 0; i < ui->datasetTree->topLevelItemCount(); ++i) {
    QTreeWidgetItem *item = ui->datasetTree->topLevelItem(i);
    if (item->data(0, IdRole).toString() == datasetId)
      return item;
  }
  return nullptr;
}
