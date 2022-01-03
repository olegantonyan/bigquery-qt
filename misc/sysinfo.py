import sys

import PySide6 as PySide


class SysInfo:
    def __init__(self):
        self.app_version = qApp.applicationVersion()
        self.py_version = '.'.join((str(i) for i in sys.version_info[0:3]))
        self.py_executable = sys.executable
        self.pyside_version = PySide.__version__
        self.qt_compiled_version = PySide.QtCore.__version__
        self.qt_runtinme_version = PySide.QtCore.qVersion()

    def __str__(self) -> str:
        return f"""\
App version:            {self.app_version}
Python version:         {self.py_version}
Python executable:      {self.py_executable}
PySide version:         {self.pyside_version}
Qt version (compiled):  {self.qt_compiled_version}
Qt version (running):   {self.qt_runtinme_version}"""
