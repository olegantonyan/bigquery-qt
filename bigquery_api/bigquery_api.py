import asyncio

from google.cloud import bigquery


class BigQueryAPI:
    def __init__(self, project: str):
        self._client = bigquery.Client(project=project)

    def datasets(self):
        return self._client.list_datasets()

    def tables(self, dataset: str):
        return self._client.list_tables(dataset)
