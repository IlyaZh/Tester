#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <QComboBox>

#include "globals.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    void preloadData();
    ~SettingsDialog();

private slots:
    void openDBSelectDialog();
    void accept();
    void reject();

signals:
    void updateTesterNameSignal();

private:
    Ui::SettingsDialog *ui;
    void setConnections();
    QSettings *qsettings;
};

#endif // SETTINGSDIALOG_H
