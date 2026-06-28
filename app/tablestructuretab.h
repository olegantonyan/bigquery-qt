#ifndef TABLESTRUCTURETAB_H
#define TABLESTRUCTURETAB_H

#include <QFutureWatcher>
#include <QWidget>

#include "bigqueryclient.h"

class QLabel;
class QPlainTextEdit;
class QPushButton;
class QSplitter;
class QTableWidget;
class QTreeWidget;
class QTreeWidgetItem;

class TableStructureTab : public QWidget
{
  Q_OBJECT

public:
  explicit TableStructureTab(QString projectId, QString datasetId, QString tableId,
                             QWidget *parent = nullptr);

  QString projectId() const { return m_projectId; }
  QString datasetId() const { return m_datasetId; }
  QString tableId() const { return m_tableId; }

public slots:
  void refresh();

signals:
  void status(const QString &message);

private slots:
  void displayDetails();

private:
  void addSchemaRows(QTreeWidgetItem *parentItem, const QList<SchemaField> &fields);

  QString m_projectId;
  QString m_datasetId;
  QString m_tableId;

  QLabel *m_header;
  QPushButton *m_refreshButton;
  QTableWidget *m_propsTable;
  QTreeWidget *m_schemaTree;
  QPlainTextEdit *m_viewSql;
  QSplitter *m_splitter;
  QFutureWatcher<TableDetails> m_watcher;
};

#endif // TABLESTRUCTURETAB_H
