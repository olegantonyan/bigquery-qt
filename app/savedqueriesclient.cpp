#include "savedqueriesclient.h"

#include "google/cloud/dataform/v1/dataform_client.h"

#include <QDateTime>
#include <QTimeZone>

namespace {
namespace df = ::google::cloud::dataform_v1;
namespace dfp = ::google::cloud::dataform::v1;

QString regionFromResource(const QString &resourceName) {
  const QStringList parts = resourceName.split(QLatin1Char('/'));
  const int i = parts.indexOf(QStringLiteral("locations"));
  return (i >= 0 && i + 1 < parts.size()) ? parts.at(i + 1) : QString();
}

QString formatTimestamp(const ::google::protobuf::Timestamp &ts) {
  if (ts.seconds() == 0 && ts.nanos() == 0)
    return QString();
  return QDateTime::fromSecsSinceEpoch(ts.seconds(), QTimeZone::UTC).toString(Qt::ISODate);
}
} // namespace

SavedQueriesClient::SavedQueriesClient(QString projectId)
  : m_projectId(std::move(projectId))
{
}

SavedQueryListResult SavedQueriesClient::list() const
{
  SavedQueryListResult result;
  auto client = df::DataformClient(df::MakeDataformConnection());

  dfp::ListRepositoriesRequest req;
  req.set_parent("projects/" + m_projectId.toStdString() + "/locations/-");

  for (auto const &repo : client.ListRepositories(req)) {
    if (!repo) {
      result.error = QString::fromStdString(repo.status().message());
      return result;
    }
    const auto &labels = repo->labels();
    const auto it = labels.find("single-file-asset-type");
    if (it == labels.end() || it->second != "sql")
      continue;

    SavedQuery q;
    q.resourceName = QString::fromStdString(repo->name());
    q.id = q.resourceName.section(QLatin1Char('/'), -1);
    q.displayName = QString::fromStdString(repo->display_name());
    q.region = regionFromResource(q.resourceName);
    if (repo->has_create_time())
      q.createTime = formatTimestamp(repo->create_time());
    result.queries << q;
  }

  result.ok = true;
  return result;
}

FileContentResult SavedQueriesClient::getSql(const QString &repository) const
{
  FileContentResult result;
  auto client = df::DataformClient(df::MakeDataformConnection());

  dfp::QueryRepositoryDirectoryContentsRequest dirReq;
  dirReq.set_name(repository.toStdString());

  std::string filePath;
  for (auto const &entry : client.QueryRepositoryDirectoryContents(dirReq)) {
    if (!entry) {
      result.error = QString::fromStdString(entry.status().message());
      return result;
    }
    if (entry->entry_case() == dfp::DirectoryEntry::kFile) {
      filePath = entry->file();
      break;
    }
  }
  if (filePath.empty()) {
    result.error = QStringLiteral("No file found in saved query repository.");
    return result;
  }

  dfp::ReadRepositoryFileRequest readReq;
  readReq.set_name(repository.toStdString());
  readReq.set_path(filePath);

  auto response = client.ReadRepositoryFile(readReq);
  if (!response) {
    result.error = QString::fromStdString(response.status().message());
    return result;
  }

  result.content = QString::fromStdString(response->contents());
  result.ok = true;
  return result;
}
