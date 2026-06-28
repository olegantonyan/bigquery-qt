#ifndef SAVEDQUERYTAB_H
#define SAVEDQUERYTAB_H

#include <QFutureWatcher>
#include <QWidget>

#include "savedqueriesclient.h"

class QLabel;
class QPlainTextEdit;
class QSplitter;
class QTableWidget;

class SavedQueryTab : public QWidget
{
  Q_OBJECT

public:
  explicit SavedQueryTab(QString projectId, QString repository, QString region,
                         QString displayName, QWidget *parent = nullptr);

signals:
  void status(const QString &message);

private slots:
  void displayContent();

private:
  QString m_projectId;
  QString m_repository;
  QString m_region;
  QString m_displayName;

  QLabel *m_header;
  QTableWidget *m_metaTable;
  QPlainTextEdit *m_sql;
  QSplitter *m_splitter;
  QFutureWatcher<FileContentResult> m_watcher;
};

#endif // SAVEDQUERYTAB_H
