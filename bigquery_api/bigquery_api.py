import asyncio

from google.cloud import bigquery

import bigquery_api.dataset as dataset
import bigquery_api.table as table
import bigquery_api.project as project


class BigQueryAPI:
    def __init__(self, project: str):
        self._client = bigquery.Client(project=project)

    def datasets(self) -> list[dataset.Dataset]:
        return [dataset.Dataset(i.reference) for i in self._client.list_datasets()]

    def tables(self, dataset: str) -> list[table.Table]:
        return [table.Table(i.reference) for i in self._client.list_tables(dataset)]

    def projects(self) -> list[project.Project]:
        return [project.Project(i) for i in self._client.list_projects()]
