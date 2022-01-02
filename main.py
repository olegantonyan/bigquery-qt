import os
import sys

from PySide6.QtWidgets import QApplication
from PySide6.QtCore import QStandardPaths

import mainwindow
import misc.config as config


def mkconfig():
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
    app.setApplicationVersion('0.0.1')
    app.setApplicationDisplayName(f"{qApp.applicationName()} {qApp.applicationVersion()}")

    cfg = mkconfig()

    win = mainwindow.MainWindow(cfg)
    win.show()

    sys.exit(app.exec())


if __name__ == "__main__":
    main()
