#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QByteArray>
#include <QMainWindow>

#include "bigqueryclient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QPoint;
class QTreeWidgetItem;
class QueryTab;
class TableStructureTab;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

protected:
  void closeEvent(QCloseEvent *event) override;

private slots:
  void loadDatasets();
  void loadTables(QTreeWidgetItem *datasetItem);
  void prefillSelect(QTreeWidgetItem *tableItem);
  void showTreeContextMenu(const QPoint &pos);
  void loadSavedQueries();
  void openSavedQuery(QTreeWidgetItem *item);
  void newTab();
  void closeTab(int index);

private:
  QueryTab *addTab(const QString &sql = QString());
  TableStructureTab *openStructureTab(const QString &project, const QString &dataset,
                                      const QString &table);
  QueryTab *currentTab() const;
  void restoreUi();
  void saveUi();
  void loadProjectHistory();
  void rememberProject(const QString &project);
  QTreeWidgetItem *findDatasetItem(const QString &datasetId) const;

  Ui::MainWindow *ui;
  QByteArray m_splitterState;
  int m_tabCounter = 0;
};
#endif // MAINWINDOW_H
