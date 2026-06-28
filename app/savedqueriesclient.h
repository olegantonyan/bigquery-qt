#ifndef SAVEDQUERIESCLIENT_H
#define SAVEDQUERIESCLIENT_H

#include <QList>
#include <QString>

struct SavedQuery {
  QString id;
  QString resourceName;
  QString displayName;
  QString region;
  QString createTime;
};

struct SavedQueryListResult {
  bool ok = false;
  QString error;
  QList<SavedQuery> queries;
};

struct FileContentResult {
  bool ok = false;
  QString error;
  QString content;
};

class SavedQueriesClient {
public:
  explicit SavedQueriesClient(QString projectId);

  SavedQueryListResult list() const;
  FileContentResult getSql(const QString &repository) const;

private:
  QString m_projectId;
};

#endif // SAVEDQUERIESCLIENT_H
