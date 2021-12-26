from PySide6.QtCore import QObject, QByteArray, Qt
from PySide6.QtWidgets import QMainWindow, QPushButton, QVBoxLayout, QHBoxLayout, QSplitter, QTreeView, QTextEdit, QWidget, QToolBar
from PySide6.QtGui import QAction, QIcon

import configuration
import dumpster
import tables.controller


class MainWindow(QMainWindow):
    def __init__(self, config: configuration.Configuration):
        super(self.__class__, self).__init__()
        self.config = config

        vertical_layout = QVBoxLayout()

        self.main_splitter = QSplitter()
        self.secondary_splitter = QSplitter(Qt.Vertical)
        tree_view = QTreeView()
        text_edit_query = QTextEdit()
        text_edit_results = QTextEdit()

        self.secondary_splitter.addWidget(text_edit_query)
        self.secondary_splitter.addWidget(text_edit_results)

        self.main_splitter.addWidget(tree_view)
        self.main_splitter.addWidget(self.secondary_splitter)

        vertical_layout.addWidget(self.main_splitter)

        central_widget = QWidget()
        central_widget.setLayout(vertical_layout)
        self.setCentralWidget(central_widget)

        self.tables_controller = tables.controller.Controller(config=config, view=tree_view)

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
        if self.config.window_geometry is not None:
            self.restoreGeometry(dumpster.qbytearray_from_str(self.config.window_geometry))
        if self.config.window_state is not None:
            self.restoreState(dumpster.qbytearray_from_str(self.config.window_state))
        if self.config.main_splitter_sizes is not None:
            self.main_splitter.setSizes(self.config.main_splitter_sizes)
        if self.config.secondary_splitter_sizes is not None:
            self.secondary_splitter.setSizes(self.config.secondary_splitter_sizes)


    def _save_geometry(self):
        self.config.window_geometry = dumpster.qbytearray_to_str(self.saveGeometry())
        self.config.window_state = dumpster.qbytearray_to_str(self.saveState())
        self.config.secondary_splitter_sizes = self.secondary_splitter.sizes()
        self.config.sync()

    def the_button_was_clicked(self):
        qApp.aboutQt()
