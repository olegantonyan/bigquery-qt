#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QApplication::setOrganizationName(QStringLiteral("bigquery-qt"));
  QApplication::setApplicationName(QStringLiteral("bigquery-qt"));
  MainWindow w;
  w.show();
  return QApplication::exec();
}
