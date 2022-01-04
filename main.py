import os
import sys
import argparse

from PySide6.QtWidgets import QApplication
from PySide6.QtCore import QStandardPaths

import mainwindow
import misc.config as config
import misc.sysinfo as sysinfo
import version
import bigquery_api.bigquery_api as bigquery_api


def mkconfig(custom_path: str = None) -> config.Config:
    if custom_path is not None:
        return config.Config(custom_path)

    config_file_dir = os.path.join(QStandardPaths.writableLocation(QStandardPaths.ConfigLocation), qApp.applicationName())
    if not os.path.exists(config_file_dir):
        os.makedirs(config_file_dir)
    config_file_path = os.path.join(config_file_dir, 'config.yaml')
    if not os.path.exists(config_file_path):
        open(config_file_path, mode='a').close()
    return config.Config(config_file_path)


def mkbq(cfg: config.Config) -> bigquery_api.BigQueryAPI:
    return bigquery_api.BigQueryAPI(cfg.project)


def mkqtapp() -> QApplication:
    app = QApplication()
    app.setApplicationName('bigquery-qt')
    app.setApplicationVersion(version.VERSION)
    app.setApplicationDisplayName(f"{qApp.applicationName()} {qApp.applicationVersion()}")
    return app


def parse_cli() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description='Google BigQuery desktop frontend')
    parser.add_argument('--config', default=None, help='path to config file')
    parser.add_argument('--version', action='version', version=qApp.applicationDisplayName())
    # https://docs.python.org/3/library/argparse.html#argparse.ArgumentParser.add_argument
    return parser.parse_args()


def main():
    app = mkqtapp()
    args = parse_cli()
    cfg = mkconfig(args.config)

    print(f"using config file {cfg.path}")
    print(sysinfo.SysInfo())

    print(f"connecting to BigQuery project {cfg.project}")
    bq = mkbq(cfg)

    win = mainwindow.MainWindow(cfg, bq)
    win.show()

    sys.exit(app.exec())


if __name__ == "__main__":
    main()
