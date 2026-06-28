#include "tablestructuretab.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QtConcurrent>

TableStructureTab::TableStructureTab(QString projectId, QString datasetId, QString tableId,
                                     QWidget *parent)
  : QWidget(parent)
  , m_projectId(std::move(projectId))
  , m_datasetId(std::move(datasetId))
  , m_tableId(std::move(tableId))
{
  m_header = new QLabel(
      QStringLiteral("`%1.%2.%3`").arg(m_projectId, m_datasetId, m_tableId));
  m_header->setTextInteractionFlags(Qt::TextSelectableByMouse);
  m_refreshButton = new QPushButton(tr("Refresh"));

  auto *headerRow = new QHBoxLayout;
  headerRow->setContentsMargins(4, 4, 4, 0);
  headerRow->addWidget(m_header, 1);
  headerRow->addWidget(m_refreshButton, 0);

  m_propsTable = new QTableWidget(0, 2);
  m_propsTable->setHorizontalHeaderLabels({tr("Property"), tr("Value")});
  m_propsTable->verticalHeader()->setVisible(false);
  m_propsTable->horizontalHeader()->setStretchLastSection(true);
  m_propsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_propsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  m_schemaTree = new QTreeWidget;
  m_schemaTree->setColumnCount(4);
  m_schemaTree->setHeaderLabels({tr("Name"), tr("Type"), tr("Mode"), tr("Description")});

  m_viewSql = new QPlainTextEdit;
  m_viewSql->setReadOnly(true);
  m_viewSql->setVisible(false);

  m_splitter = new QSplitter(Qt::Vertical);
  m_splitter->addWidget(m_propsTable);
  m_splitter->addWidget(m_schemaTree);
  m_splitter->addWidget(m_viewSql);
  m_splitter->setStretchFactor(0, 0);
  m_splitter->setStretchFactor(1, 1);
  m_splitter->setStretchFactor(2, 1);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addLayout(headerRow);
  layout->addWidget(m_splitter);

  connect(m_refreshButton, &QPushButton::clicked, this, &TableStructureTab::refresh);
  connect(&m_watcher, &QFutureWatcher<TableDetails>::finished, this,
          &TableStructureTab::displayDetails);

  refresh();
}

void TableStructureTab::refresh()
{
  m_refreshButton->setEnabled(false);
  emit status(tr("Loading table…"));
  BigQueryClient client(m_projectId);
  m_watcher.setFuture(QtConcurrent::run(
      [client, dataset = m_datasetId, table = m_tableId]() { return client.getTable(dataset, table); }));
}

void TableStructureTab::addSchemaRows(QTreeWidgetItem *parentItem, const QList<SchemaField> &fields)
{
  for (const SchemaField &f : fields) {
    auto *item = parentItem ? new QTreeWidgetItem(parentItem)
                            : new QTreeWidgetItem(m_schemaTree);
    item->setText(0, f.name);
    item->setText(1, f.type);
    item->setText(2, f.mode);
    item->setText(3, f.description);
    if (!f.fields.isEmpty())
      addSchemaRows(item, f.fields);
  }
}

void TableStructureTab::displayDetails()
{
  const TableDetails d = m_watcher.result();
  m_refreshButton->setEnabled(true);

  if (!d.ok) {
    emit status(tr("Load failed"));
    QMessageBox::warning(this, tr("Load failed"), d.error);
    return;
  }

  m_propsTable->setRowCount(d.properties.size());
  for (int row = 0; row < d.properties.size(); ++row) {
    m_propsTable->setItem(row, 0, new QTableWidgetItem(d.properties.at(row).name));
    m_propsTable->setItem(row, 1, new QTableWidgetItem(d.properties.at(row).value));
  }
  m_propsTable->resizeColumnToContents(0);

  m_schemaTree->clear();
  addSchemaRows(nullptr, d.schema);
  for (int i = 0; i < m_schemaTree->topLevelItemCount(); ++i)
    m_schemaTree->topLevelItem(i)->setExpanded(true);
  for (int c = 0; c < m_schemaTree->columnCount(); ++c)
    m_schemaTree->resizeColumnToContents(c);

  if (d.viewQuery.isEmpty()) {
    m_viewSql->setVisible(false);
  } else {
    m_viewSql->setPlainText(d.viewQuery);
    m_viewSql->setVisible(true);
  }

  emit status(tr("Loaded"));
}
