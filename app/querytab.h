#ifndef QUERYTAB_H
#define QUERYTAB_H

#include <QFutureWatcher>
#include <QWidget>

#include <functional>

#include "bigqueryclient.h"

class QPlainTextEdit;
class QPushButton;
class QSplitter;
class QTableWidget;

class QueryTab : public QWidget
{
  Q_OBJECT

public:
  explicit QueryTab(std::function<QString()> projectProvider, QWidget *parent = nullptr);

  QString queryText() const;
  void setQueryText(const QString &sql);
  QByteArray splitterState() const;
  void restoreSplitterState(const QByteArray &state);

public slots:
  void runQuery();

signals:
  void status(const QString &message);

private slots:
  void displayResults();

private:
  std::function<QString()> m_projectProvider;
  QPlainTextEdit *m_queryEdit;
  QPushButton *m_runButton;
  QTableWidget *m_resultsTable;
  QSplitter *m_splitter;
  QFutureWatcher<QueryResult> m_watcher;
};

#endif // QUERYTAB_H
