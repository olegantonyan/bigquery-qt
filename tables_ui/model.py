import asyncio

from PySide6.QtGui import QStandardItemModel, QStandardItem
from google.cloud import bigquery

import misc.config as config


class Model(QStandardItemModel):
    def __init__(self, cfg: config.Config, *args):
        super(self.__class__, self).__init__(*args)
        self.cfg = cfg

    def reload(self) -> None:
        if self.cfg.project is None:
            return

        client = bigquery.Client(project=self.cfg.project)
        for dataset in client.list_datasets():
            dataset_row = Item(f"{dataset.dataset_id}")
            for table in client.list_tables(dataset.dataset_id):
                table_row = Item(f"{table.table_id}")
                dataset_row.appendRow(table_row)
            self.appendRow(dataset_row)


class Item(QStandardItem):
    def __init__(self, *args):
        super(self.__class__, self).__init__(*args)
