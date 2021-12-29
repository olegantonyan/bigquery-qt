from PySide6.QtWidgets import QTreeView, QAbstractItemView
from PySide6.QtCore import QItemSelectionModel

import configuration
import tables.model as model


class Controller:
    def __init__(self, config: configuration.Configuration, view: QTreeView):
        self.view = view
        self.config = config

        self.view.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.view.setUniformRowHeights(True)

        self.model = model.Model(config=self.config)
        self.model.setHorizontalHeaderLabels(['col1', 'col2', 'col3'])
        self.view.setModel(self.model)

        for i in range(3):
            parent1 = model.Item('Family {}. Some long status text for sp'.format(i))
            for j in range(3):
                child1 = model.Item('Child {}'.format(i*3+j))
                child2 = model.Item('row: {}, col: {}'.format(i, j+1))
                child3 = model.Item('row: {}, col: {}'.format(i, j+2))
                parent1.appendRow([child1, child2, child3])
            self.model.appendRow(parent1)
            # span container columns
            self.view.setFirstColumnSpanned(i, view.rootIndex(), True)
        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        # expand third container
        index = self.model.indexFromItem(parent1)
        self.view.expand(index)
        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        # select last row
        selmod = self.view.selectionModel()
        index2 = self.model.indexFromItem(child3)
        selmod.select(index2, QItemSelectionModel.Select|QItemSelectionModel.Rows)
