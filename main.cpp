#include "mainwindow.h"
#include "itemdialog.h"
#include "settingsdialog.h"
#include "globals.h"

#include <QApplication>

/*
 * 1. Объедени классы csvexport и printer
 * 2. Вывод sql запросов в окна ошибок
 * 3. Сделай сохранение последнего выбранного пути для экспорта данных и импорта
 * 4. Логирование действий
 * 5. Справка
*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    NewItemDialog addDialog;
    SettingsDialog settingsDialog;

    w.show();

    QObject::connect(&addDialog, SIGNAL(addNewChip(ChipInfo_t)), &w, SLOT(addChipSlot(ChipInfo_t)));
    QObject::connect(&w, SIGNAL(openNewChipWindow()), &addDialog, SLOT(openWindow()));
    QObject::connect(&w, SIGNAL(openEditChipWindow(ChipInfo_t)), &addDialog, SLOT(openWindow(ChipInfo_t)));
    QObject::connect(&addDialog, SIGNAL(chipEditComplete(ChipInfo_t)), &w, SLOT(updateChipData(ChipInfo_t)));
    QObject::connect(&w, SIGNAL(openSettingsWindow()), &settingsDialog, SLOT(show()));
    QObject::connect(&settingsDialog, SIGNAL(updateTesterNameSignal()), &w, SLOT(settingsUpdated()));

    return a.exec();
}
