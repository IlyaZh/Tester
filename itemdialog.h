#ifndef ITEMDIALOG_H
#define ITEMDIALOG_H

#include <QDialog>
#include <QComboBox>
#include "globals.h"

namespace Ui {
class NewItemDialog;
}

class NewItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewItemDialog(QWidget *parent = 0);
    ~NewItemDialog();

public slots:
    void setEndDate(int hours);
    void updateEndDate();
    void openWindow();
    void openWindow(ChipInfo_t);

private slots:
    void accept();
    void reject();
    void setCurrentDateTimeStart();

signals:
    void addNewChip(ChipInfo_t);
    void chipEditComplete(ChipInfo_t);

private:
    Ui::NewItemDialog *ui;
    void setConnections();
};

#endif // NEWITEMDIALOG_H
