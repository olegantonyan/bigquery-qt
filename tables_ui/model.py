from PySide6.QtGui import QStandardItemModel, QStandardItem

import bigquery_api.bigquery_api as bigquery_api


class Model(QStandardItemModel):
    def __init__(self, bq: bigquery_api.BigQueryAPI, *args):
        super(self.__class__, self).__init__(*args)
        self.bq = bq

    def reload(self) -> None:
        self.clear()
        self.load()

    def load(self):
        parent = self.invisibleRootItem()
        for dataset in self.bq.datasets():
            dataset_row = Item(dataset.dataset_id, "dataset")
            for table in self.bq.tables(dataset.dataset_id):
                table_row = Item(table.table_id, "table")
                dataset_row.appendRow(table_row)
            parent.appendRow(dataset_row)

class Item(QStandardItem):
    def __init__(self, text: str, type: str):
        super(self.__class__, self).__init__(text)
        self.type = type
