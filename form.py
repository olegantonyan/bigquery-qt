import os

from PySide6.QtCore import QFile
from PySide6.QtUiTools import QUiLoader


def load(form_filename, parent):
    loader = QUiLoader()
    path = os.path.join(os.path.dirname(__file__), form_filename)
    ui_file = QFile(path)
    ui_file.open(QFile.ReadOnly)
    result = loader.load(ui_file)
    ui_file.close()
    return result
