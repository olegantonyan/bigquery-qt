import os
import sys

import PySide6 as PySide
from PySide6.QtWidgets import QApplication
from PySide6.QtCore import QStandardPaths

import mainwindow
import misc.config as config
import version


def mkconfig(custom_path: str = None):
    if custom_path is not None:
        return config.Config(custom_path)

    config_file_dir = os.path.join(QStandardPaths.writableLocation(QStandardPaths.ConfigLocation), qApp.applicationName())
    if not os.path.exists(config_file_dir):
        os.makedirs(config_file_dir)
    config_file_path = os.path.join(config_file_dir, 'config.yaml')
    if not os.path.exists(config_file_path):
        open(config_file_path, mode='a').close()
    return config.Config(config_file_path)

def main():
    app = QApplication(sys.argv)
    app.setApplicationName('bigquery-qt')
    app.setApplicationVersion(version.VERSION)
    app.setApplicationDisplayName(f"{qApp.applicationName()} {qApp.applicationVersion()}")

    cfg = mkconfig()

    print(f"App version:            {qApp.applicationVersion()}")
    print(f"Config file:            {cfg.path}")
    print(f"Python version:         {'.'.join((str(i) for i in sys.version_info[0:3]))}")
    print(f"Python executable:      {sys.executable}")
    print(f"PySide version:         {PySide.__version__}")
    print(f"Qt version (compiled):  {PySide.QtCore.__version__}")
    print(f"Qt version (running):   {PySide.QtCore.qVersion()}")

    win = mainwindow.MainWindow(cfg)
    win.show()

    sys.exit(app.exec())


if __name__ == "__main__":
    main()
