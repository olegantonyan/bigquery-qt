import asyncio

from google.cloud import bigquery

import bigquery_api.dataset as dataset
import bigquery_api.table as table


class BigQueryAPI:
    def __init__(self, project: str):
        self._client = bigquery.Client(project=project)

    def datasets(self):
        return [dataset.Dataset(i.reference) for i in self._client.list_datasets()]

    def tables(self, dataset: str):
        return [table.Table(i.reference) for i in self._client.list_tables(dataset)]
