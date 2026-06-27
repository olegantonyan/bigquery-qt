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

QueryResult bqRunQuery(const QString &projectId, const QString &sql);
DatasetListResult bqListDatasets(const QString &projectId);
TableListResult bqListTables(const QString &projectId, const QString &datasetId);

#endif // BIGQUERYCLIENT_H
