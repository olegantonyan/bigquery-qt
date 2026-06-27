#include "querytab.h"

#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QtConcurrent>

QueryTab::QueryTab(std::function<QString()> projectProvider, QWidget *parent)
  : QWidget(parent)
  , m_projectProvider(std::move(projectProvider))
{
  m_queryEdit = new QPlainTextEdit;
  m_queryEdit->setPlainText(QStringLiteral("SELECT 1 AS n, 'hello' AS greeting"));
  m_runButton = new QPushButton(tr("Run query"));
  m_resultsTable = new QTableWidget;

  auto *editorPane = new QWidget;
  auto *editorLayout = new QVBoxLayout(editorPane);
  editorLayout->setContentsMargins(0, 0, 0, 0);
  editorLayout->addWidget(m_queryEdit);
  editorLayout->addWidget(m_runButton);

  m_splitter = new QSplitter(Qt::Vertical);
  m_splitter->addWidget(editorPane);
  m_splitter->addWidget(m_resultsTable);
  m_splitter->setStretchFactor(0, 0);
  m_splitter->setStretchFactor(1, 1);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_splitter);

  connect(m_runButton, &QPushButton::clicked, this, &QueryTab::runQuery);
  connect(&m_watcher, &QFutureWatcher<QueryResult>::finished, this, &QueryTab::displayResults);
}

QString QueryTab::queryText() const
{
  return m_queryEdit->toPlainText();
}

void QueryTab::setQueryText(const QString &sql)
{
  m_queryEdit->setPlainText(sql);
}

QByteArray QueryTab::splitterState() const
{
  return m_splitter->saveState();
}

void QueryTab::restoreSplitterState(const QByteArray &state)
{
  if (!state.isEmpty())
    m_splitter->restoreState(state);
}

void QueryTab::runQuery()
{
  const QString project = m_projectProvider ? m_projectProvider().trimmed() : QString();
  const QString sql = m_queryEdit->toPlainText().trimmed();
  if (project.isEmpty()) {
    QMessageBox::warning(this, tr("Missing project"), tr("Enter a GCP billing project id."));
    return;
  }
  if (sql.isEmpty())
    return;

  m_runButton->setEnabled(false);
  emit status(tr("Running query…"));
  m_watcher.setFuture(QtConcurrent::run(bqRunQuery, project, sql));
}

void QueryTab::displayResults()
{
  const QueryResult r = m_watcher.result();
  m_runButton->setEnabled(true);

  if (!r.ok) {
    emit status(tr("Query failed"));
    QMessageBox::warning(this, tr("Query failed"), r.error);
    return;
  }

  m_resultsTable->clear();
  m_resultsTable->setColumnCount(r.headers.size());
  m_resultsTable->setHorizontalHeaderLabels(r.headers);
  m_resultsTable->setRowCount(r.rows.size());
  for (int row = 0; row < r.rows.size(); ++row) {
    const QStringList &cells = r.rows.at(row);
    for (int col = 0; col < cells.size(); ++col)
      m_resultsTable->setItem(row, col, new QTableWidgetItem(cells.at(col)));
  }

  emit status(tr("%n row(s)", "", static_cast<int>(r.rows.size())));
}
