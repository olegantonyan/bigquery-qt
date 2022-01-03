from PySide6.QtGui import QStandardItemModel, QStandardItem

import bigquery_api.bigquery_api as bigquery_api


class Model(QStandardItemModel):
    def __init__(self, bq: bigquery_api.BigQueryAPI, *args):
        super(self.__class__, self).__init__(*args)
        self.bq = bq

    def reload(self) -> None:
        self.clear()

        for dataset in self.bq.datasets():
            dataset_row = Item(f"{dataset.dataset_id}")
            for table in self.bq.tables(dataset.dataset_id):
                table_row = Item(f"{table.table_id}")
                dataset_row.appendRow(table_row)
            self.appendRow(dataset_row)


class Item(QStandardItem):
    def __init__(self, *args):
        super(self.__class__, self).__init__(*args)
