from PySide6.QtWidgets import QTreeView, QAbstractItemView, QComboBox
from PySide6.QtCore import QObject, QItemSelectionModel, QModelIndex

import misc.config as config
import tables_ui.model as model
import bigquery_api.bigquery_api as bigquery_api


class Controller(QObject):
    def __init__(self, cfg: config.Config, bq: bigquery_api.BigQueryAPI, view: QTreeView, project_combobox: QComboBox):
        super().__init__()
        
        self.view = view
        self.project_combobox = project_combobox
        self.cfg = cfg
        self.bq = bq

        self.view.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.view.setUniformRowHeights(True)
        self.view.setHeaderHidden(True)
        self.view.setEditTriggers(QAbstractItemView.NoEditTriggers)

        self.model = model.Model(bq=self.bq)
        self.view.setModel(self.model)

        self.view.clicked.connect(self._item_clicked)

        self.model.reload()

        self.project_combobox.addItems([i.id() for i in bq.projects()])
        self.project_combobox.setCurrentText(cfg.project)
        self.project_combobox.activated.connect(self._project_changed)

    def _item_clicked(self, index: QModelIndex) -> None:
        item = self.model.itemFromIndex(index)
        print(item.type + " -- " + item.text())

    def _project_changed(self, index: int) -> None:
        self.cfg.project = self.project_combobox.itemText(index)
        self.bq.reload(self.cfg.project)
        self.model.reload()
