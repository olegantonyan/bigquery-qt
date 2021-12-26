from PySide6.QtCore import QObject, QByteArray
from PySide6.QtWidgets import QMainWindow, QPushButton, QVBoxLayout
from PySide6.QtGui import QAction

import form
import configuration
import dumpster


class MainWindowController(QObject):
    def __init__(self):
        super(MainWindowController, self).__init__()
        self.ui = form.load("mainwindow.ui")

        self.ui.pushButton.clicked.connect(self.the_button_was_clicked)

    def show(self):
        self.ui.show()

    def the_button_was_clicked(self):
        qApp.aboutQt()



class MainWindow(QMainWindow):
    def __init__(self, config: configuration.Configuration):
        super(self.__class__, self).__init__()
        self.config = config

        self.button1 = QPushButton('button1', self)

        self.button1.clicked.connect(self.the_button_was_clicked)

        #menubar = self.menuBar()
        #file = menubar.addMenu('&File')
        #self.exit = QAction('Exit', self)
        #self.exit.triggered.connect(self.close)
        #file.addAction(self.exit)

        # create the status bar
        self.statusBar()

        self.setWindowTitle('MainWindow')

        if config.window_geometry is not None:
            self.restoreGeometry(dumpster.qbytearray_from_str(config.window_geometry))
        if config.window_state is not None:
            self.restoreState(dumpster.qbytearray_from_str(config.window_state))

    def closeEvent(self, _event):
        self.config.window_geometry = dumpster.qbytearray_to_str(self.saveGeometry())
        self.config.window_state = dumpster.qbytearray_to_str(self.saveState())
        self.config.sync()

    def the_button_was_clicked(self):
        qApp.aboutQt()
