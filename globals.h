#ifndef MAIN_H
#define MAIN_H

#include <QString>
#include <QStringList>
#include <QDateTime>

#define ORG_NAME "ZHILAB"
#define APP_NAME "ESTester"
#define MAJOR_VERSION   1
#define MINOR_VERSION   41
const QString appTitle = "Испытатель " + QString::number(MAJOR_VERSION) + "." + QString::number(MINOR_VERSION);
enum filter_t {noFilter = 0, onlyInProgress, willDoneToday, onlyNoDone, onlyDone};

typedef struct {
    QString chipName;
    QString map;
    int count;
    qint64 startTime;
    qint64 duration;
    qint64 endTime;
    QString device;
    QString comment;
    bool isDone;
    int id;
} ChipInfo_t;

typedef enum {ASK, MINIMIZE, EXIT} closeAction_t;
const QStringList closeActionItems = {"Спросить о действии", "Свернуть", "Выйти из программы"};

#define TABLE_COL_CHIP        0
#define TABLE_COL_MAP         1
#define TABLE_COL_COUNT       2
#define TABLE_COL_START_TIME  3
#define TABLE_COL_DURATION    4
#define TABLE_COL_END_TIME    5
#define TABLE_COL_DEVICE      6
#define TABLE_COL_COMMENT     7
#define TABLE_COL_ISDONE      8
#define TABLE_COL_TESTER      9
#define TABLE_COL_ID          10

#define TABLE_CHIP        1
#define TABLE_MAP         2
#define TABLE_COUNT       3
#define TABLE_DURATION    4
#define TABLE_START_TIME  5
#define TABLE_END_TIME    6
#define TABLE_DEVICE      7
#define TABLE_COMMENT     8
#define TABLE_ISDONE      9
#define TABLE_TESTER      10
#define TABLE_ID          11

#define TESTER_NAME       qsettings->value("userSettings/userName", "NoName").toString()

const QString DB_TABLE_NAME = "chipList";

const QString EDIT_DATE_FORMAT = "dd MMM yyyy ddd HH:mm";
const QString CURRENT_DATE_FORMAT = "dd MMM yyyy ddd HH:mm:ss";
const QString DATE_FORMAT = "dd MMM yyyy (ddd) HH:mm";
const QString ONLY_DAY_OF_WEEK = "ddd";

#define TIMER_DEFAULT_PERIOD 600 // sec
#define IMPORT_EXPORT_DATA_SEPARATOR "|||"

#define CLOSE_PROGRAM   0
#define HIDE_PROGRAM    1

#define DUMP_IMPORT_HEADERS_LINE    0
#define DUMP_IMPORT_DATA_LINE       1

#endif // MAIN_H
