from PySide6.QtCore import QObject, QByteArray, Qt
from PySide6.QtWidgets import QMainWindow, QPushButton, QVBoxLayout, QHBoxLayout, QSplitter, QTreeView, QTextEdit, QWidget, QToolBar, QComboBox
from PySide6.QtGui import QAction, QIcon

import misc.config as config
import misc.dumpster as dumpster
import tables_ui.controller
import bigquery_api.bigquery_api as bigquery_api


class MainWindow(QMainWindow):
    def __init__(self, cfg: config.Config, bq: bigquery_api.BigQueryAPI):
        super(self.__class__, self).__init__()
        self.cfg = cfg

        vertical_layout = QVBoxLayout()

        self.main_splitter = QSplitter()
        self.secondary_splitter = QSplitter(Qt.Vertical)
        tree_view = QTreeView()
        text_edit_query = QTextEdit()
        text_edit_results = QTextEdit()
        project_combobox = QComboBox()

        tree_layout_widget = QWidget()
        tree_and_combo_layout = QVBoxLayout(tree_layout_widget)
        tree_and_combo_layout.setContentsMargins(0, 0, 0, 0)
        tree_and_combo_layout.addWidget(tree_view)
        tree_and_combo_layout.addWidget(project_combobox)

        self.secondary_splitter.addWidget(text_edit_query)
        self.secondary_splitter.addWidget(text_edit_results)

        self.main_splitter.addWidget(tree_layout_widget)
        self.main_splitter.addWidget(self.secondary_splitter)

        vertical_layout.addWidget(self.main_splitter)

        central_widget = QWidget()
        central_widget.setLayout(vertical_layout)
        self.setCentralWidget(central_widget)

        self.tables_controller = tables_ui.controller.Controller(cfg=self.cfg, bq=bq, view=tree_view, project_combobox=project_combobox)

        #main_toolbar = QToolBar()
        #main_toolbar.setObjectName("main_toolbar")
        #self.addToolBar(main_toolbar)

        #action1 = QAction("ololo", main_toolbar)
        #action1.setStatusTip("this is ololo")
        #main_toolbar.addAction(action1)

        #self.button1 = QPushButton('button1', self)

        #self.button1.clicked.connect(self.the_button_was_clicked)

        #menubar = self.menuBar()
        #file = menubar.addMenu('&File')
        #self.exit = QAction('Exit', self)
        #self.exit.triggered.connect(self.close)
        #file.addAction(self.exit)

        # create the status bar
        self.statusBar()

        self.setWindowTitle('MainWindow')
        self._restore_geometry()


    def closeEvent(self, _event):
        self._save_geometry()

    def _restore_geometry(self):
        if self.cfg.window_geometry is not None:
            self.restoreGeometry(dumpster.qbytearray_from_str(self.cfg.window_geometry))
        if self.cfg.window_state is not None:
            self.restoreState(dumpster.qbytearray_from_str(self.cfg.window_state))
        if self.cfg.main_splitter_sizes is not None:
            self.main_splitter.setSizes(self.cfg.main_splitter_sizes)
        if self.cfg.secondary_splitter_sizes is not None:
            self.secondary_splitter.setSizes(self.cfg.secondary_splitter_sizes)


    def _save_geometry(self):
        self.cfg.window_geometry = dumpster.qbytearray_to_str(self.saveGeometry())
        self.cfg.window_state = dumpster.qbytearray_to_str(self.saveState())
        self.cfg.main_splitter_sizes = self.main_splitter.sizes()
        self.cfg.secondary_splitter_sizes = self.secondary_splitter.sizes()
        self.cfg.sync()

    def the_button_was_clicked(self):
        qApp.aboutQt()
