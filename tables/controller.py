from PySide6.QtWidgets import QTreeView

from google.cloud import bigquery

import configuration


class Controller:
    def __init__(self, config: configuration.Configuration, view: QTreeView):
        self.view = view
        self.config = config

        if self.config.project is not None:
            client = bigquery.Client(project=self.config.project)
            for i in client.list_datasets():
                print(i.dataset_id)
