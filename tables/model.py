from PySide6.QtGui import QStandardItemModel, QStandardItem

from google.cloud import bigquery

import configuration


class Model(QStandardItemModel):
    def __init__(self, config: configuration.Configuration, *args):
        super(self.__class__, self).__init__(*args)
        #if config.project is not None:
        #    client = bigquery.Client(project=config.project)
        #    print(list(client.list_datasets()))


class Item(QStandardItem):
    def __init__(self, *args):
        super(self.__class__, self).__init__(*args)
