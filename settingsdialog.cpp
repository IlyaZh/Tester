#include "settingsdialog.h"
#include "ui_settingsdialog.h"


SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    qsettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope, ORG_NAME, APP_NAME);

    setConnections();
    preloadData();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::setConnections() {
    connect(ui->selectDBButton, SIGNAL(clicked(bool)), this, SLOT(openDBSelectDialog()));
}

void SettingsDialog::openDBSelectDialog() {
    QFileDialog dialog(this);
    dialog.setNameFilter("SQLite Database files (*.sqlite)");
    dialog.setFileMode(QFileDialog::AnyFile);
    //qDebug() << dialog.getOpenFileName(this, "Открытие файла базы данных", ,"SQLite Database files (*.sqlite)");
    if(dialog.exec()) {
        ui->dbNameEdit->setText(dialog.selectedFiles().at(0));
    }
}

void SettingsDialog::preloadData() {
    ui->testerNameEdit->setText(qsettings->value("userSettings/userName", "Фамилия И.О.").toString());
    ui->dbNameEdit->setText(qsettings->value("userSettings/dbName", QString(qApp->applicationDirPath()) + "/data.sqlite").toString());
    ui->closeActionBox->addItems(closeActionItems);
    ui->closeActionBox->setCurrentIndex(qsettings->value("userSettings/closeAction", ASK).toInt());
    ui->timerPeriodSpinBox->setValue(qsettings->value("userSettings/timerPeriod", TIMER_DEFAULT_PERIOD).toInt());
    ui->filterBox->setCurrentIndex(qsettings->value("userSettings/defaultFilter", noFilter).toInt());
}

void SettingsDialog::accept() {
    qsettings->setValue("userSettings/userName", ui->testerNameEdit->text());
    qsettings->setValue("userSettings/dbName", ui->dbNameEdit->text());
    qsettings->setValue("userSettings/closeAction", ui->closeActionBox->currentIndex());
    qsettings->setValue("userSettings/timerPeriod", ui->timerPeriodSpinBox->value());
    qsettings->setValue("userSettings/defaultFilter", ui->filterBox->currentIndex());

    emit updateTesterNameSignal();
    this->hide();
}

void SettingsDialog::reject() {
    this->hide();
}
