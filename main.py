# This Python file uses the following encoding: utf-8
import os
import sys

from PySide6.QtWidgets import QApplication, QMainWindow
from PySide6.QtCore import QFile, QObject
from PySide6.QtUiTools import QUiLoader

class MainWindowController(QObject):
    def __init__(self):
        super(MainWindowController, self).__init__()
        self.window = self.load_ui()

        self.window.pushButton.clicked.connect(self.the_button_was_clicked)

    def load_ui(self):
        loader = QUiLoader()
        path = os.path.join(os.path.dirname(__file__), "form.ui")
        ui_file = QFile(path)
        ui_file.open(QFile.ReadOnly)
        window = loader.load(ui_file)
        ui_file.close()
        return window

    def show(self):
        self.window.show()

    def the_button_was_clicked(self):
        qApp.aboutQt()


if __name__ == "__main__":
    app = QApplication([])
    widget = MainWindowController()
    widget.show()
    sys.exit(app.exec())
