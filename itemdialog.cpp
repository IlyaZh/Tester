#include "itemdialog.h"
#include "ui_newitemdialog.h"

#include <QDebug>

NewItemDialog::NewItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewItemDialog)
{
    ui->setupUi(this);

    setConnections();

    ui->MCdateTimeStart->setDate(QDate::currentDate());
    setEndDate(0);
}

NewItemDialog::~NewItemDialog()
{
    delete ui;
}

void NewItemDialog::setConnections() {
    connect(ui->MCduration, SIGNAL(valueChanged(int)), this, SLOT(setEndDate(int)));
    connect(ui->MCdateTimeStart, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(updateEndDate()));
    connect(ui->setCurrentDateTimeButton, SIGNAL(released()), this, SLOT(setCurrentDateTimeStart()));
}

void NewItemDialog::setEndDate(int hours) {
    ui->MCdateTimeEnd->setDateTime(ui->MCdateTimeStart->dateTime().addSecs(3600*hours));
    //ui->endDOWLabel->setText(ui->MCdateTimeEnd->dateTime().toString(ONLY_DAY_OF_WEEK));
    //ui->startDOWLabel->setText(ui->MCdateTimeStart->dateTime().toString(ONLY_DAY_OF_WEEK));
}

void NewItemDialog::updateEndDate() {
    setEndDate(ui->MCduration->value());
}

void NewItemDialog::reject() {
    this->hide();
}

void NewItemDialog::accept() {
    ChipInfo_t data;

    data.chipName = ui->MCchipName->text();
    data.count = ui->MCcount->value();
    data.map = ui->MCmap->text();
    data.duration = ui->MCduration->value();
    data.startTime = ui->MCdateTimeStart->dateTime().toSecsSinceEpoch();
    data.endTime = ui->MCdateTimeEnd->dateTime().toSecsSinceEpoch();
    data.device = ui->MCdevice->text();
    data.comment = ui->MCcomment->toPlainText();
    data.isDone = ui->MCtestIsDone->isChecked();
    data.id = ui->MCid->text().toInt();

    if(data.id == -1) {
        emit addNewChip(data);
    } else {
        emit chipEditComplete(data);
    }

    this->hide();
}

void NewItemDialog::openWindow() {
    ui->MCchipName->setText("");
    ui->MCmap->setText("");
    ui->MCcount->setValue(0);
    ui->MCduration->setValue(0);
    ui->MCdevice->setText("");
    ui->MCcomment->setPlainText("");
    ui->MCdateTimeStart->setDate(QDate::currentDate());
    ui->MCid->setText(QString::number(-1));
    ui->MCtestIsDone->setChecked(false);
    ui->MCtestIsDone->setVisible(false);
    ui->MCtestIsDoneLabel->setVisible(false);

    this->show();
}

void NewItemDialog::openWindow(ChipInfo_t chip) {
    ui->MCchipName->setText(chip.chipName);
    ui->MCmap->setText(chip.map);
    ui->MCcount->setValue(chip.count);
    ui->MCduration->setValue(chip.duration);
    QDateTime tmp;
    tmp.setSecsSinceEpoch(chip.startTime);
    ui->MCdateTimeStart->setDateTime(tmp);
    ui->MCdevice->setText(chip.device);
    ui->MCcomment->setPlainText(chip.comment);
    ui->MCtestIsDone->setChecked(chip.isDone);


    ui->MCid->setText(QString::number(chip.id));

    ui->MCtestIsDone->setVisible(true);
    ui->MCtestIsDoneLabel->setVisible(true);

    this->show();
}

void NewItemDialog::setCurrentDateTimeStart() {
    ui->MCdateTimeStart->setDateTime(QDateTime::currentDateTime());
}
