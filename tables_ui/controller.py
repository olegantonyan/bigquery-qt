from PySide6.QtWidgets import QTreeView, QAbstractItemView
from PySide6.QtCore import QItemSelectionModel

import misc.config as config
import tables_ui.model as model


class Controller:
    def __init__(self, cfg: config.Config, view: QTreeView):
        self.view = view
        self.cfg = cfg

        self.view.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.view.setUniformRowHeights(True)
        self.view.setHeaderHidden(True)
        self.view.setEditTriggers(QAbstractItemView.NoEditTriggers)

        self.model = model.Model(cfg=self.cfg)
        #self.model.setHorizontalHeaderLabels(['col1', 'col2', 'col3'])
        self.view.setModel(self.model)

        self.model.reload()
        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        # expand third container
        #index = self.model.indexFromItem(parent1)
        #self.view.expand(index)
        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        # select last row
        #selmod = self.view.selectionModel()
        #index2 = self.model.indexFromItem(child3)
        #selmod.select(index2, QItemSelectionModel.Select|QItemSelectionModel.Rows)
