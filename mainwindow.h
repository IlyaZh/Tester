#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QDate>
#include <QDebug>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QMessageBox>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QPoint>
#include <QList>
#include <QMenu>
#include <QAction>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QCursor>
#include <QCloseEvent>
#include <QInputDialog>
#include <QSettings>
#include <QTimer>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QFileDialog>
#include <QThread>
#include <QRegExp>

#include "globals.h"
#include "csvexport.h"
#include "printer.h"
#include "downloader.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void setTimer();
    void savePositionSettings();
    void loadPositionSettings();
    ~MainWindow();

signals:
    void openNewChipWindow();
    void openEditChipWindow(ChipInfo_t);
    void openSettingsWindow();
    //void quitApp();

public slots:
    //void saveDataSlot();
    void reloadDataSlot();
    void addChipSlot(ChipInfo_t);
    void checkName();
    void checkForEndedItems();
    void openSettingsWindowSlot();
    void export2CSV();
    void immediateQuit();
    void exportProgress(int);
    void exportFinishedStatus(bool status);
    void copyRowSlot();
    void exportRowSlot();
    void importRowSlot();
    void reCalcTime();
    void settingsUpdated();
//    void deleteChipSlot();

private slots:
    void contextMenuTable(QPoint);
    void addChipWindowSlot();
    void updateChipData(ChipInfo_t);
    void editChip(QModelIndex);
    void editChip(int);
    void setItemCompliteActionClicked();
    void editActionClicked();
    void deleteActionClicked();
    // tray icon slots
    void changeEvent(QEvent *);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void trayActionExecute();
    void setTrayIconActions();
    void showTrayIcon();
    void closeEvent(QCloseEvent *);
    void moveEvent(QMoveEvent*);
    void resizeEvent(QResizeEvent*);
    void setFilter(int);
    void resetFilter();
    void oneSecTimerSlot();
    void printData();
    void saveData2Html();


private:
    int itemTimerValue;
    Ui::MainWindow *ui;
    QSettings *qsettings;
    int dbFilter;
    QIcon trayImage, activeTrayImage;
    Printer *printer;
    void setConnections();
    void connectToDB();
    QSqlDatabase sdb;
    QSqlTableModel *tableModel;
    QMenu *menu;
    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QTimer itemCheckTimer;
    QTimer oneSecTimer;
    int rightClickedIndex;
    //QString testerName;
    void setTableModel();

};
#endif // MAINWINDOW_H
