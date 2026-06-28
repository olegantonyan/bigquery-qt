#include "savedquerytab.h"

#include <QHeaderView>
#include <QLabel>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QtConcurrent>

namespace {
void addMetaRow(QTableWidget *table, const QString &name, const QString &value)
{
  if (value.isEmpty())
    return;
  const int row = table->rowCount();
  table->insertRow(row);
  table->setItem(row, 0, new QTableWidgetItem(name));
  table->setItem(row, 1, new QTableWidgetItem(value));
}
} // namespace

SavedQueryTab::SavedQueryTab(QString projectId, QString repository, QString region,
                             QString displayName, QWidget *parent)
  : QWidget(parent)
  , m_projectId(std::move(projectId))
  , m_repository(std::move(repository))
  , m_region(std::move(region))
  , m_displayName(std::move(displayName))
{
  m_header = new QLabel(m_displayName);
  m_header->setTextInteractionFlags(Qt::TextSelectableByMouse);

  m_metaTable = new QTableWidget(0, 2);
  m_metaTable->setHorizontalHeaderLabels({tr("Property"), tr("Value")});
  m_metaTable->verticalHeader()->setVisible(false);
  m_metaTable->horizontalHeader()->setStretchLastSection(true);
  m_metaTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_metaTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  addMetaRow(m_metaTable, tr("Name"), m_displayName);
  addMetaRow(m_metaTable, tr("Region"), m_region);
  addMetaRow(m_metaTable, tr("Resource"), m_repository);
  m_metaTable->resizeColumnToContents(0);

  m_sql = new QPlainTextEdit;
  m_sql->setReadOnly(true);

  m_splitter = new QSplitter(Qt::Vertical);
  m_splitter->addWidget(m_metaTable);
  m_splitter->addWidget(m_sql);
  m_splitter->setStretchFactor(0, 0);
  m_splitter->setStretchFactor(1, 1);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_header);
  layout->addWidget(m_splitter);

  connect(&m_watcher, &QFutureWatcher<FileContentResult>::finished, this,
          &SavedQueryTab::displayContent);

  emit status(tr("Loading saved query…"));
  SavedQueriesClient client(m_projectId);
  const QString repo = m_repository;
  m_watcher.setFuture(QtConcurrent::run([client, repo]() { return client.getSql(repo); }));
}

void SavedQueryTab::displayContent()
{
  const FileContentResult r = m_watcher.result();
  if (!r.ok) {
    emit status(tr("Load failed"));
    m_sql->setPlainText(r.error);
    return;
  }
  m_sql->setPlainText(r.content);
  emit status(tr("Loaded"));
}
