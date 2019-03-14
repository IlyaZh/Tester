#ifndef CSVEXPORT_H
#define CSVEXPORT_H

#include <QObject>
#include <QString>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QRegExp>

class CSVexport : public QObject
{
    Q_OBJECT
public:
    CSVexport(QSqlTableModel *mod, QString filename = "database.csv");
    //~CSVexport(;)

public slots:
    void exportData();
signals:
    void isDone(bool isEnd);
    void percent(int percent);

private:
    QSqlTableModel *tModel;
    QString fileName;
    uint rows;
    uint rowsDone;
    uint progress;
};

#endif // CSVEXPORT_H
