#include "bigqueryclient.h"

#include "google/cloud/bigquerycontrol/v2/dataset_client.h"
#include "google/cloud/bigquerycontrol/v2/job_client.h"
#include "google/cloud/bigquerycontrol/v2/table_client.h"

#include <google/protobuf/struct.pb.h>

namespace {

QString valueToString(const google::protobuf::Value &v) {
  using google::protobuf::Value;
  switch (v.kind_case()) {
  case Value::kStringValue:
    return QString::fromStdString(v.string_value());
  case Value::kNumberValue:
    return QString::number(v.number_value());
  case Value::kBoolValue:
    return v.bool_value() ? QStringLiteral("true") : QStringLiteral("false");
  case Value::kNullValue:
  case Value::KIND_NOT_SET:
    return QString();
  default:
    return QString::fromStdString(v.ShortDebugString());
  }
}

} // namespace

QueryResult bqRunQuery(const QString &projectId, const QString &sql) {
  namespace bqc = ::google::cloud::bigquerycontrol_v2;
  namespace bq2 = ::google::cloud::bigquery::v2;

  QueryResult result;

  auto client = bqc::JobServiceClient(bqc::MakeJobServiceConnectionRest());

  bq2::PostQueryRequest request;
  request.set_project_id(projectId.toStdString());
  auto *q = request.mutable_query_request();
  q->set_query(sql.toStdString());
  q->mutable_use_legacy_sql()->set_value(false);
  q->mutable_max_results()->set_value(1000);

  auto response = client.Query(request);
  if (!response) {
    result.error = QString::fromStdString(response.status().message());
    return result;
  }

  const auto &schema = response->schema();
  for (int i = 0; i < schema.fields_size(); ++i)
    result.headers << QString::fromStdString(schema.fields(i).name());

  for (const auto &row : response->rows()) {
    QStringList cells;
    const auto f = row.fields().find("f");
    if (f != row.fields().end()) {
      for (const auto &cell : f->second.list_value().values()) {
        const auto &fields = cell.struct_value().fields();
        const auto v = fields.find("v");
        cells << (v != fields.end() ? valueToString(v->second) : QString());
      }
    }
    result.rows << cells;
  }

  result.ok = true;
  return result;
}

DatasetListResult bqListDatasets(const QString &projectId) {
  namespace bqc = ::google::cloud::bigquerycontrol_v2;
  namespace bq2 = ::google::cloud::bigquery::v2;

  DatasetListResult result;

  auto client = bqc::DatasetServiceClient(bqc::MakeDatasetServiceConnectionRest());

  bq2::ListDatasetsRequest request;
  request.set_project_id(projectId.toStdString());
  request.mutable_max_results()->set_value(1000);

  for (auto const &dataset : client.ListDatasets(request)) {
    if (!dataset) {
      result.error = QString::fromStdString(dataset.status().message());
      return result;
    }
    DatasetInfo info;
    info.id = QString::fromStdString(dataset->dataset_reference().dataset_id());
    info.friendlyName = QString::fromStdString(dataset->friendly_name().value());
    info.location = QString::fromStdString(dataset->location());
    result.datasets << info;
  }

  result.ok = true;
  return result;
}

TableListResult bqListTables(const QString &projectId, const QString &datasetId) {
  namespace bqc = ::google::cloud::bigquerycontrol_v2;
  namespace bq2 = ::google::cloud::bigquery::v2;

  TableListResult result;

  auto client = bqc::TableServiceClient(bqc::MakeTableServiceConnectionRest());

  bq2::ListTablesRequest request;
  request.set_project_id(projectId.toStdString());
  request.set_dataset_id(datasetId.toStdString());
  request.mutable_max_results()->set_value(1000);

  for (auto const &table : client.ListTables(request)) {
    if (!table) {
      result.error = QString::fromStdString(table.status().message());
      return result;
    }
    TableInfo info;
    info.id = QString::fromStdString(table->table_reference().table_id());
    info.type = QString::fromStdString(table->type());
    result.tables << info;
  }

  result.ok = true;
  return result;
}
