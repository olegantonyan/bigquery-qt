import os
import sys

from PySide6.QtWidgets import QApplication
from PySide6.QtCore import QStandardPaths

import mainwindow
import misc.config as config
import misc.sysinfo as sysinfo
import version
import bigquery_api.bigquery_api as bigquery_api


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
    print(f"using config file {cfg.path}")
    print(sysinfo.SysInfo())

    bq = bigquery_api.BigQueryAPI(cfg.project)

    win = mainwindow.MainWindow(cfg, bq)
    win.show()

    sys.exit(app.exec())


if __name__ == "__main__":
    main()
