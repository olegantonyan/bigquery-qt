#include "bigqueryclient.h"

#include "google/cloud/bigquerycontrol/v2/dataset_client.h"
#include "google/cloud/bigquerycontrol/v2/job_client.h"
#include "google/cloud/bigquerycontrol/v2/table_client.h"

#include <google/protobuf/struct.pb.h>

#include <QDateTime>
#include <QLocale>
#include <QTimeZone>

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

SchemaField convertField(const ::google::cloud::bigquery::v2::TableFieldSchema &f) {
  SchemaField sf;
  sf.name = QString::fromStdString(f.name());
  sf.type = QString::fromStdString(f.type());
  sf.mode = f.mode().empty() ? QStringLiteral("NULLABLE") : QString::fromStdString(f.mode());
  if (f.has_description())
    sf.description = QString::fromStdString(f.description().value());
  for (const auto &child : f.fields())
    sf.fields << convertField(child);
  return sf;
}

QString formatBytes(qint64 bytes) {
  static const char *units[] = {"B", "KiB", "MiB", "GiB", "TiB", "PiB"};
  double value = static_cast<double>(bytes);
  int unit = 0;
  while (value >= 1024.0 && unit < 5) {
    value /= 1024.0;
    ++unit;
  }
  return unit == 0 ? QStringLiteral("%1 B").arg(bytes)
                   : QStringLiteral("%1 %2").arg(value, 0, 'f', 2).arg(QLatin1String(units[unit]));
}

QString formatTsMs(qint64 ms) {
  return QDateTime::fromMSecsSinceEpoch(ms, QTimeZone::UTC).toString(Qt::ISODate);
}

void addProp(QList<TableProperty> &props, const QString &name, const QString &value) {
  if (!value.isEmpty())
    props << TableProperty{name, value};
}

} // namespace

BigQueryClient::BigQueryClient(QString projectId)
  : m_projectId(std::move(projectId))
{
}

QueryResult BigQueryClient::runQuery(const QString &sql) const {
  namespace bqc = ::google::cloud::bigquerycontrol_v2;
  namespace bq2 = ::google::cloud::bigquery::v2;

  QueryResult result;

  auto client = bqc::JobServiceClient(bqc::MakeJobServiceConnectionRest());

  bq2::PostQueryRequest request;
  request.set_project_id(m_projectId.toStdString());
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

DatasetListResult BigQueryClient::listDatasets() const {
  namespace bqc = ::google::cloud::bigquerycontrol_v2;
  namespace bq2 = ::google::cloud::bigquery::v2;

  DatasetListResult result;

  auto client = bqc::DatasetServiceClient(bqc::MakeDatasetServiceConnectionRest());

  bq2::ListDatasetsRequest request;
  request.set_project_id(m_projectId.toStdString());
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

TableListResult BigQueryClient::listTables(const QString &datasetId) const {
  namespace bqc = ::google::cloud::bigquerycontrol_v2;
  namespace bq2 = ::google::cloud::bigquery::v2;

  TableListResult result;

  auto client = bqc::TableServiceClient(bqc::MakeTableServiceConnectionRest());

  bq2::ListTablesRequest request;
  request.set_project_id(m_projectId.toStdString());
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

TableDetails BigQueryClient::getTable(const QString &datasetId, const QString &tableId) const {
  namespace bqc = ::google::cloud::bigquerycontrol_v2;
  namespace bq2 = ::google::cloud::bigquery::v2;

  TableDetails result;
  result.projectId = m_projectId;
  result.datasetId = datasetId;
  result.tableId = tableId;

  auto client = bqc::TableServiceClient(bqc::MakeTableServiceConnectionRest());

  bq2::GetTableRequest request;
  request.set_project_id(m_projectId.toStdString());
  request.set_dataset_id(datasetId.toStdString());
  request.set_table_id(tableId.toStdString());
  request.set_view(bq2::GetTableRequest::FULL);

  auto response = client.GetTable(request);
  if (!response) {
    result.error = QString::fromStdString(response.status().message());
    return result;
  }

  const auto &t = *response;
  result.tableType = QString::fromStdString(t.type());

  auto &props = result.properties;
  addProp(props, QStringLiteral("Type"), result.tableType);
  addProp(props, QStringLiteral("Location"), QString::fromStdString(t.location()));
  if (t.creation_time() != 0)
    addProp(props, QStringLiteral("Created"), formatTsMs(t.creation_time()));
  if (t.last_modified_time() != 0)
    addProp(props, QStringLiteral("Last modified"),
            formatTsMs(static_cast<qint64>(t.last_modified_time())));
  if (t.has_expiration_time())
    addProp(props, QStringLiteral("Expires"), formatTsMs(t.expiration_time().value()));
  if (t.has_num_rows())
    addProp(props, QStringLiteral("Rows"),
            QLocale().toString(static_cast<qulonglong>(t.num_rows().value())));
  if (t.has_num_total_logical_bytes())
    addProp(props, QStringLiteral("Logical bytes (total)"), formatBytes(t.num_total_logical_bytes().value()));
  if (t.has_num_active_logical_bytes())
    addProp(props, QStringLiteral("Logical bytes (active)"), formatBytes(t.num_active_logical_bytes().value()));
  if (t.has_num_long_term_logical_bytes())
    addProp(props, QStringLiteral("Logical bytes (long-term)"), formatBytes(t.num_long_term_logical_bytes().value()));
  if (t.has_num_total_physical_bytes())
    addProp(props, QStringLiteral("Physical bytes (total)"), formatBytes(t.num_total_physical_bytes().value()));
  if (t.has_num_partitions())
    addProp(props, QStringLiteral("Partitions"), QString::number(t.num_partitions().value()));
  if (t.has_time_partitioning()) {
    QString tp = QString::fromStdString(t.time_partitioning().type());
    if (t.time_partitioning().has_field())
      tp += QStringLiteral(" on %1").arg(QString::fromStdString(t.time_partitioning().field().value()));
    if (t.time_partitioning().has_expiration_ms())
      tp += QStringLiteral(", expires after %1 ms").arg(t.time_partitioning().expiration_ms().value());
    addProp(props, QStringLiteral("Time partitioning"), tp);
  }
  if (t.has_range_partitioning())
    addProp(props, QStringLiteral("Range partitioning"),
            QString::fromStdString(t.range_partitioning().field()));
  if (t.has_clustering()) {
    QStringList fields;
    for (int i = 0; i < t.clustering().fields_size(); ++i)
      fields << QString::fromStdString(t.clustering().fields(i));
    addProp(props, QStringLiteral("Clustering"), fields.join(QStringLiteral(", ")));
  }
  if (t.has_require_partition_filter())
    addProp(props, QStringLiteral("Require partition filter"),
            t.require_partition_filter().value() ? QStringLiteral("yes") : QStringLiteral("no"));
  if (t.has_table_constraints()) {
    const auto &tc = t.table_constraints();
    if (tc.has_primary_key()) {
      QStringList cols;
      for (int i = 0; i < tc.primary_key().columns_size(); ++i)
        cols << QString::fromStdString(tc.primary_key().columns(i));
      addProp(props, QStringLiteral("Primary key"), cols.join(QStringLiteral(", ")));
    }
    if (tc.foreign_keys_size() > 0)
      addProp(props, QStringLiteral("Foreign keys"), QString::number(tc.foreign_keys_size()));
  }
  if (t.has_friendly_name())
    addProp(props, QStringLiteral("Friendly name"), QString::fromStdString(t.friendly_name().value()));
  if (t.has_description())
    addProp(props, QStringLiteral("Description"), QString::fromStdString(t.description().value()));
  for (const auto &label : t.labels())
    addProp(props, QStringLiteral("label: %1").arg(QString::fromStdString(label.first)),
            QString::fromStdString(label.second));
  addProp(props, QStringLiteral("ETag"), QString::fromStdString(t.etag()));

  for (const auto &field : t.schema().fields())
    result.schema << convertField(field);

  if (t.has_view())
    result.viewQuery = QString::fromStdString(t.view().query());
  else if (t.has_materialized_view())
    result.viewQuery = QString::fromStdString(t.materialized_view().query());

  result.ok = true;
  return result;
}
