#ifndef BIGQUERYCLIENT_H
#define BIGQUERYCLIENT_H

#include <QList>
#include <QString>
#include <QStringList>

struct QueryResult {
  bool ok = false;
  QString error;
  QStringList headers;
  QList<QStringList> rows;
};

struct DatasetInfo {
  QString id;
  QString friendlyName;
  QString location;
};

struct TableInfo {
  QString id;
  QString type;
};

struct SchemaField {
  QString name;
  QString type;
  QString mode;
  QString description;
  QList<SchemaField> fields;
};

struct TableProperty {
  QString name;
  QString value;
};

struct TableDetails {
  bool ok = false;
  QString error;
  QString projectId;
  QString datasetId;
  QString tableId;
  QString tableType;
  QList<TableProperty> properties;
  QList<SchemaField> schema;
  QString viewQuery;
};

struct DatasetListResult {
  bool ok = false;
  QString error;
  QList<DatasetInfo> datasets;
};

struct TableListResult {
  bool ok = false;
  QString error;
  QList<TableInfo> tables;
};

class BigQueryClient {
public:
  explicit BigQueryClient(QString projectId);

  QueryResult runQuery(const QString &sql) const;
  DatasetListResult listDatasets() const;
  TableListResult listTables(const QString &datasetId) const;
  TableDetails getTable(const QString &datasetId, const QString &tableId) const;

private:
  QString m_projectId;
};

#endif // BIGQUERYCLIENT_H
