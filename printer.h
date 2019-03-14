#ifndef PRINTER_H
#define PRINTER_H

#include <QObject>
#include <QSqlTableModel>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QRegExp>
#include "globals.h"

class Printer : public QObject
{
    Q_OBJECT
public:
    explicit Printer(QObject *parent = 0);
    void printContent();
    bool saveContent2Html();
    void prepareHtmlSqlModel(QSqlTableModel *model);

signals:

public slots:
    //void prepareHtmlSqlModel(QSqlTableModel *model);

private:
    QPrinter *device;
    QString Pcontent;
};

#endif // PRINTER_H
