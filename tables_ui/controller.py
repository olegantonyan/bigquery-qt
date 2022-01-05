from PySide6.QtWidgets import QTreeView, QAbstractItemView, QComboBox
from PySide6.QtCore import QItemSelectionModel, QModelIndex

import misc.config as config
import tables_ui.model as model
import bigquery_api.bigquery_api as bigquery_api


class Controller:
    def __init__(self, cfg: config.Config, bq: bigquery_api.BigQueryAPI, view: QTreeView, project_combobox: QComboBox):
        self.view = view
        self.project_combobox = project_combobox
        self.cfg = cfg

        self.view.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.view.setUniformRowHeights(True)
        self.view.setHeaderHidden(True)
        self.view.setEditTriggers(QAbstractItemView.NoEditTriggers)

        self.model = model.Model(bq=bq)
        self.view.setModel(self.model)

        self.view.clicked.connect(self.item_clicked)

        self.model.reload()

        self.project_combobox.addItems([i.id() for i in bq.projects()])

    def item_clicked(self, index: QModelIndex) -> None:
        item = self.model.itemFromIndex(index)
        print(item.type + " -- " + item.text())
