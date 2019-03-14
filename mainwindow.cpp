#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    qsettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope, ORG_NAME, APP_NAME);

    setConnections();
    loadPositionSettings();
    checkName();
    connectToDB();



    rightClickedIndex = -1;

    showTrayIcon();
    setTrayIconActions();
    reloadDataSlot();
    resetFilter();

    oneSecTimerSlot();
}

MainWindow::~MainWindow()
{
    sdb.close();
    delete ui;
}

void MainWindow::setConnections() {
    connect(ui->addChipButton, SIGNAL(clicked(bool)), this, SLOT(addChipWindowSlot()));
    connect(ui->reloadChipButton, SIGNAL(clicked(bool)), this, SLOT(reloadDataSlot()));
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuTable(QPoint)));
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editChip(QModelIndex)));
    connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(reCalcTime()));
    connect(ui->settingsButton, SIGNAL(clicked(bool)), this, SLOT(openSettingsWindowSlot()));
    connect(ui->filterBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setFilter(int)));
    connect(ui->resetSortFiltersButton, SIGNAL(clicked(bool)), this, SLOT(resetFilter()));
    connect(ui->importMCButton, SIGNAL(clicked(bool)), this, SLOT(importRowSlot()));

    // Обработка меню
    connect(ui->settingsAction, SIGNAL(triggered(bool)), this, SLOT(openSettingsWindowSlot()));
    connect(ui->exportCSVAction, SIGNAL(triggered(bool)), this, SLOT(export2CSV()));
    connect(ui->printAction, SIGNAL(triggered(bool)), this, SLOT(printData()));
    connect(ui->exportHtmlAction, SIGNAL(triggered(bool)), this, SLOT(saveData2Html()));
    connect(ui->minimazeAction, SIGNAL(triggered(bool)), this, SLOT(hide()));
    connect(ui->exitAction, SIGNAL(triggered(bool)), this, SLOT(immediateQuit()));
    connect(ui->addItemAction, SIGNAL(triggered(bool)), this, SLOT(addChipWindowSlot()));
    connect(ui->importMCAction, SIGNAL(triggered(bool)), this, SLOT(importRowSlot()));
    connect(ui->reloadDBAction, SIGNAL(triggered(bool)), this, SLOT(reloadDataSlot()));
    connect(ui->resetSortFiltersAction, SIGNAL(triggered(bool)), this, SLOT(resetFilter()));


    //connect(ui->tableView->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(saveButtonEnable(QModelIndex,QModelIndex)));
}

void MainWindow::savePositionSettings() {
    qsettings->beginGroup("MainWindow");
    qsettings->setValue("geometry", saveGeometry());
    qsettings->setValue("state", saveState());
    qsettings->setValue("maximized", isMaximized());
    if (!isMaximized()) {
        qsettings->setValue("pos", pos());
        qsettings->setValue("size", size());
    }
    qsettings->endGroup();
}

void MainWindow::loadPositionSettings() {
    qsettings->beginGroup("MainWindow");
    restoreGeometry(qsettings->value("geometry", saveGeometry()).toByteArray());
    restoreState(qsettings->value("state", saveState()).toByteArray());
    move(qsettings->value("pos", pos()).toPoint());
    resize(qsettings->value("size", size()).toSize());
    if(qsettings->value("maximized", isMaximized()).toBool())
        showMaximized();

    qsettings->endGroup();

    setWindowTitle(appTitle);
}

void MainWindow::addChipWindowSlot() {
    emit openNewChipWindow();
}
/*void MainWindow::saveDataSlot() {
    QMessageBox::StandardButton answer;
    answer = QMessageBox::question(this, "Test", "Записать изменения?", QMessageBox::Yes|QMessageBox::No);
    if (answer) {


        if(!tableModel->submitAll()) {
            // обработка ошибки
            QMessageBox msgBox;
            msgBox.setText("Не удаётся записать изменения");
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
        } else {
            // всё ок
            ui->statusBar->showMessage("Изменения сохранены");
        }
    }
}*/

void MainWindow::reloadDataSlot() {
    tableModel->setTable(DB_TABLE_NAME);

    QString query = "";
    QDateTime tmpDT;
    switch(dbFilter) {
        case noFilter:
        default:
            query.clear();
            break;
        case onlyInProgress:
            query = "isDone=0"; // AND endTime > "+QString::number(QDateTime::currentDateTime().toSecsSinceEpoch());
            break;
        case willDoneToday:
            tmpDT.setDate(QDate::currentDate());
            query = "isDone=0 AND endTime < "+QString::number(tmpDT.addDays(1).toSecsSinceEpoch()); // endTime > "+QString::number(tmpDT.toSecsSinceEpoch())+" AND
            break;
        case onlyNoDone:
            query = "isDone=0 AND endTime < "+QString::number(QDateTime::currentDateTime().toSecsSinceEpoch());
        break;
        case onlyDone:
            query = "isDone=1";
        break;

    }

    tableModel->setFilter(query);
    tableModel->setSort(TABLE_COL_ID, Qt::DescendingOrder);

    tableModel->select();
    tableModel->setHeaderData(0, Qt::Horizontal, "Изделие");
    tableModel->setHeaderData(1, Qt::Horizontal, "МК");
    tableModel->setHeaderData(2, Qt::Horizontal, "Количество, шт");
    tableModel->setHeaderData(3, Qt::Horizontal, "Время начала");
    tableModel->setHeaderData(4, Qt::Horizontal, "Длительность, ч");
    tableModel->setHeaderData(5, Qt::Horizontal, "Время окончания");
    tableModel->setHeaderData(6, Qt::Horizontal, "Оборудование");
    tableModel->setHeaderData(7, Qt::Horizontal, "Комментарий\\режим");
    tableModel->setHeaderData(8, Qt::Horizontal, "Проведено");
    tableModel->setHeaderData(9, Qt::Horizontal, "ОИ");
    tableModel->setHeaderData(10, Qt::Horizontal, "ID");
    tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    ui->tableView->setModel(tableModel);
    ui->tableView->hideColumn(TABLE_COL_TESTER);
    ui->tableView->hideColumn(TABLE_COL_ID);

    reCalcTime();

    ui->tableView->show();

    // Заполнение сводки по таблице

    // Получение общего количество изделий
    int *itemsInProgress = new int(0);
    QSqlQuery q;
    query = "SELECT count(*) FROM " + DB_TABLE_NAME;
    if(!q.exec(query)) {
        qDebug() << "Error sql exec. Total tests query";
        (*itemsInProgress) = 0;
    } else {
        q.next();
        (*itemsInProgress) = q.value(0).toInt();
    }
    ui->totalOperations->setText(q.value(0).toString());
    // Получение количества проведённых изделий
    query = "SELECT count(*) FROM " + DB_TABLE_NAME + " WHERE isDone != 0";
    if(!q.exec(query)) {
        qDebug() << "Error sql exec. Done tests query";
    } else {
        q.next();
        (*itemsInProgress) -= q.value(0).toInt();
        ui->operationsDone->setText(q.value(0).toString());
        // Расчёт количества изделий в процессе испытаний
        ui->operationsInProgress->setText(QString::number(*itemsInProgress));
    }
    delete itemsInProgress;

    // Количество изделий, которые надо снять сегодня
    QDateTime currentDateTime;
    currentDateTime.setDate(QDate::currentDate());
    qint64 startOfDay = currentDateTime.toSecsSinceEpoch();
    qint64 endOfDay = startOfDay + 60*60*24;
    query = "SELECT count(*) FROM " + DB_TABLE_NAME + " WHERE endTime < %1 AND isDone == 0"; // endTime >= %1 AND
    query = query
           // .arg(QString::number(startOfDay))
            .arg(QString::number(endOfDay));

    if (!q.exec(query)) {
        qDebug() << "Error sql exec. Done today tests query";
        ui->doneToday->setText("-1");
    } else {
        q.next();
        if (q.value(0) > 0) {
            ui->doneToday->setText("<b>" + q.value(0).toString() + "</b>");
        } else {
            ui->doneToday->setText(q.value(0).toString());
        }
    }

    checkForEndedItems();

}

void MainWindow::reCalcTime() {
    for (uint16_t i = 0; i < tableModel->rowCount(); i++) {
        //QModelIndex posStartDate = tableModel->index(i, TABLE_COL_START_TIME, QModelIndex());
        //QModelIndex posEndDate = tableModel->index(i, TABLE_COL_END_TIME, QModelIndex());
        QModelIndex pos = tableModel->index(i, TABLE_COL_START_TIME, QModelIndex());
        //if (!tableModel->data(posIsDone).toBool()) {
        //if (!tableModel->data(posIsDone).toBool() && QDateTime(tableModel->data(posIsDone).toDateTime()).toString(DATE_FORMAT) < QDateTime::currentDateTime().toString(DATE_FORMAT)) {
           // tableModel->setData(tableModel->index(i, 1), QBrush(Qt::red), Qt::BackgroundRole);

        //}
        QDateTime tmp;
        tmp.setSecsSinceEpoch(tableModel->data(pos).toLongLong());
        tableModel->setData(pos, QVariant(tmp.toString(DATE_FORMAT)));

        pos = tableModel->index(i, TABLE_COL_END_TIME, QModelIndex());
        tmp.setSecsSinceEpoch(tableModel->data(pos).toLongLong());
        tableModel->setData(pos, QVariant(tmp.toString(DATE_FORMAT)));
    }
}

// нужен ли?? используяется ли??
/*void MainWindow::deleteChipSlot() {
    QItemSelectionModel *select =ui->tableView->selectionModel();
    QList<QModelIndex> data = select->selectedRows();
    QList<int> rows;
    for (int i = 0; i < data.length(); i++) {
        qDebug() << data.at(i).row();
        rows.append(data.at(i).row());
    }

    if(rows.length() == 0) return;

    QMessageBox::StandardButton answer;
    QString str = "Вы действительно хотите удалить строки: ";
    for (int i = 0; i < rows.length(); i++)
        str.append(QString::number(rows.at(i)+1) + " ");
    str.append("?");
    answer = QMessageBox::question(this, "Удаление", str, QMessageBox::Yes|QMessageBox::No);
    if (answer == QMessageBox::Yes) {
        qDebug() << "Вжух!";
    }

}*/

void MainWindow::addChipSlot(ChipInfo_t chip) {
    QSqlQuery q;
    QString str_insert = "INSERT INTO "+DB_TABLE_NAME+"(chip, map, count, startTime, duration, endTime, device, comment, isDone, tester) "
                "VALUES ('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10');";
    QString str = str_insert.arg(chip.chipName)
                .arg(chip.map)
                .arg(QString::number(chip.count))
                .arg(QString::number(chip.startTime))
                .arg(QString::number(chip.duration))
                .arg(QString::number(chip.endTime))
                .arg(chip.device)
                .arg(chip.comment)
                .arg(chip.isDone)
                .arg(TESTER_NAME);
//    qDebug() << "Insert query: " + str_insert;
    if(!q.exec(str)) {
        // Обработка ошибок
        QMessageBox msgBox;
        msgBox.setText("Не удалось записать данные в базу");
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setDetailedText(sdb.lastError().text() + '\n' + str_insert);
        msgBox.exec();
    }
    reloadDataSlot();
}

void MainWindow::contextMenuTable(QPoint p) {
    QMenu *menu = new QMenu(this);

    QItemSelectionModel *select = ui->tableView->selectionModel();

    menu->addAction("Проведено\\не проведено", this, SLOT(setItemCompliteActionClicked()));
    menu->addAction("Копировать строку", this, SLOT(copyRowSlot()));
    menu->addAction("Экспортировать строку", this, SLOT(exportRowSlot()));
    menu->addAction("Изменить", this, SLOT(editActionClicked()));
    menu->addAction("Удалить", this, SLOT(deleteActionClicked()));

    //qDebug() << select->hasSelection(); //check if has selection
    //qDebug() << select->selectedRows(); // return selected row(s)

    if (select->selectedRows().count() == 1) {
        QModelIndex qindex = ui->tableView->indexAt(p);
        if(!qindex.isValid()) return;

        QModelIndex pos = tableModel->index(ui->tableView->indexAt(p).row(), TABLE_COL_ID, QModelIndex());
        rightClickedIndex = tableModel->data(pos).toInt();
        menu->exec(ui->tableView->mapToGlobal(p));
    }
    delete menu;
}

void MainWindow::setItemCompliteActionClicked() {
    QSqlQuery q;
    QString query = "UPDATE " + DB_TABLE_NAME + " SET isDone = 1-isDone WHERE id=" + QString::number(rightClickedIndex);
    if(!q.exec(query)) {
        QMessageBox msgBox;
        msgBox.setText("Не удалось выполнить действие. Проблемы с базой данных");
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setDetailedText(sdb.lastError().text());
        msgBox.exec();
        return;
    }

    reloadDataSlot();
}

void MainWindow::editActionClicked() {
    editChip(rightClickedIndex);
}
void MainWindow::deleteActionClicked() {
    QSqlQuery q;
    QSqlRecord rec;
    QString str = "Вы действительно хотите удалить: ";

    QString query = "SELECT chip FROM " + DB_TABLE_NAME + " WHERE id='" + QString::number(rightClickedIndex) + "'";

    q.exec(query);
    rec = q.record();
    if (q.next()) {
       str.append(q.value(rec.indexOf("chip")).toString());
    }

    QMessageBox::StandardButton answer;

    str.append("?");

    answer = QMessageBox::question(this, "Удаление", str, QMessageBox::Yes|QMessageBox::No);
    if (answer == QMessageBox::Yes) {
        query = "DELETE FROM ";
        query.append(DB_TABLE_NAME);
        query.append(" WHERE id=");
        query.append(QString::number(rightClickedIndex));
        if (q.exec(query)) {
            reloadDataSlot();
        } else {
            QMessageBox msgBox;
            msgBox.setText("Не удалось удалить. Возможно проблемы с базой данных");
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.setDetailedText(sdb.lastError().text());
            msgBox.exec();
        }
    }
}

void MainWindow::editChip(QModelIndex index) {
    QModelIndex pos = tableModel->index(index.row(), TABLE_COL_ID, QModelIndex());
    editChip(tableModel->data(pos).toInt());
}

void MainWindow::editChip(int id) {
    ChipInfo_t chip;

    chip.id = id;

    QSqlQuery q;
    QSqlRecord rec;

    q.exec(QString("SELECT * FROM "+DB_TABLE_NAME+" WHERE id = \"%1\"").arg(QString::number(chip.id)));
    //qDebug() << q.lastQuery();
    rec = q.record();
    if(q.next()) {
        chip.chipName = q.value(rec.indexOf("chip")).toString();
        chip.map = q.value(rec.indexOf("map")).toString();
        chip.comment = q.value(rec.indexOf("comment")).toString();
        chip.count = q.value(rec.indexOf("count")).toInt();
        chip.device = q.value(rec.indexOf("device")).toString();
        chip.duration = q.value(rec.indexOf("duration")).toInt();
        chip.startTime = q.value(rec.indexOf("startTime")).toLongLong();
        chip.endTime = q.value(rec.indexOf("endTime")).toLongLong();
        chip.isDone = q.value(rec.indexOf("isDone")).toBool();
        emit openEditChipWindow(chip);
    } else {
        // Обработка ошибок
        QMessageBox msgBox;
        msgBox.setText("Не удалось загрузить данные из базы");
        msgBox.setDetailedText(sdb.lastError().text());
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }

}

void MainWindow::updateChipData(ChipInfo_t chip) {
    QSqlQuery q;
    q.prepare("UPDATE "+DB_TABLE_NAME+" SET chip=:chip, map=:map, count=:count, startTime=:startTime, duration=:duration, endTime=:endTime, device=:device, comment=:comment, isDone=:isDone WHERE id=:id");
    q.bindValue(":chip", chip.chipName);
    q.bindValue(":map", chip.map);
    q.bindValue(":count", chip.count);
    q.bindValue(":startTime", chip.startTime);
    q.bindValue(":duration", chip.duration);
    q.bindValue(":endTime", chip.endTime);
    q.bindValue(":device", chip.device);
    q.bindValue(":comment", chip.comment);
    q.bindValue(":isDone", chip.isDone);
    q.bindValue(":id", chip.id);
    if(!q.exec()) {
        // Обработка ошибок
        QMessageBox msgBox;
        msgBox.setText("Не удалось обновить данные. Ошибка базы данных");
        msgBox.setDetailedText(sdb.lastError().text());
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }

    reloadDataSlot();
    checkForEndedItems();
}

void MainWindow::showTrayIcon() {
    trayIcon = new QSystemTrayIcon(this);
    trayImage = QIcon(":/images/icon.png");
    activeTrayImage = QIcon(":/images/icon-active.png");
    trayIcon->setIcon(trayImage);
    //trayIcon->setContextMenu(trayIconMenu);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    trayIcon->show();
    //delete trayIcon;
}

void MainWindow::trayActionExecute() {
    showNormal();
    setWindowState(Qt::WindowActive);
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch(reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        this->trayActionExecute();
        break;
    case QSystemTrayIcon::Context:
        trayIconMenu->exec(QCursor::pos());
        break;
    default:
        break;
    }
}

void MainWindow::setTrayIconActions() {
    // Setting actions...
    minimizeAction = new QAction("Свернуть", this);
    restoreAction = new QAction("Восстановить", this);
    quitAction = new QAction("Выход", this);

    // Connecting actions to slots...
    connect (minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
    connect (restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));
    connect (quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    // Setting system tray's icon menu...
    trayIconMenu = new QMenu(this);
    trayIconMenu -> addAction(minimizeAction);
    trayIconMenu -> addAction(restoreAction);
    trayIconMenu -> addAction(quitAction);
}

void MainWindow::changeEvent(QEvent *event) {
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            hide();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QMainWindow::closeEvent(event);

    int answer;

    switch (qsettings->value("userSettings/closeAction", ASK).toInt()) {
    case ASK:
    default:
        answer = QMessageBox::question(this, "Вы действительно хотите выйти?", "Вы действительно хотите выйти?\nПри выходе из программы она не будет присылать уведомления", "Закрыть", "Свернуть в трей");
        if(answer == CLOSE_PROGRAM) {
            savePositionSettings();
            event->accept();
            break;
        }
    case MINIMIZE:
        event->ignore();
        hide();
        break;
    case EXIT:
        savePositionSettings();
        event->accept();
        break;
    }




    //event->ignore();
    /*if () {
        event->accept();   //подтверждаем событие - добросовестно закрываем приложение
    } else {
        event->ignore();   //игнорируем событие
    }*/
}

void MainWindow::checkForEndedItems() {
    // Получение количества изделий для которых вышел срок испытаний
    qint64 currentTime = QDateTime::currentDateTime().toSecsSinceEpoch();
    QString query = "SELECT count(*) FROM " + DB_TABLE_NAME + " WHERE (endTime < \""+QString::number(currentTime)+"\" AND isDone != 1)";
    //QString query2 = "SELECT * FROM " + DB_TABLE_NAME + " WHERE (endTime < \""+currentTime+"\" AND isDone != 1)";

    //qDebug() << query;

    QSqlQuery q;
    if(!q.exec(query)) {
        qDebug() << "Error sql exec";
        return;
    }

    /*while(q.next()) {
        QSqlRecord rec  = q.record();
        qDebug()  << q.value(rec.indexOf("chip")) << " " << q.value(rec.indexOf("endTime")) << " " << (q.value(rec.indexOf("endTime")) <= currentTime);
    }*/

    q.next();
    QVariant count = q.value(0);

    if (count.toInt() > 0) {
        QString msg = count.toString() + " изделий(я) вышло с испытаний!";
        trayIcon->showMessage("Закончились испытания!", msg, QSystemTrayIcon::Information);
        ui->operationsComplete->setText("<b>" + QString::number(count.toInt()) + "</b>");
        trayIcon->setIcon(activeTrayImage);
        trayIcon->show();
        connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(showNormal()));
    } else {
        ui->operationsComplete->setText(QString::number(count.toInt()));
        trayIcon->setIcon(trayImage);
    }

   /* q.exec(query2);
    while(q.next()) {
        QSqlRecord rec = q.record();
        qDebug() << q.value(rec.indexOf("chip")) << " " << q.value(rec.indexOf("endTime")) << " " << currentTime << " " << (q.value(rec.indexOf("endTime")) <= currentTime);
    }*/

    setTimer();
    //qDebug() << "The timer has statred";
}

void MainWindow::moveEvent(QMoveEvent *event) {
    event->accept();
    savePositionSettings();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    event->accept();
    savePositionSettings();
}

void MainWindow::settingsUpdated() {
    checkName();
    //connectToDB();
    setTableModel();
    reloadDataSlot();
}

void MainWindow::checkName() {
    bool enterNameisOk;
    QString testerName;
    if (TESTER_NAME == "NoName") {
        do {
            testerName = QInputDialog::getText(this, "Ввод данных", "Как вас зовут?", QLineEdit::Normal, "Фамилия И.О.", &enterNameisOk);
        } while(!enterNameisOk);
        qsettings->setValue("userSettings/userName", testerName);
    }

    ui->testerName->setText(TESTER_NAME);
}

void MainWindow::setTimer() {
    itemCheckTimer.singleShot(qsettings->value("userSettings/timerPeriod", TIMER_DEFAULT_PERIOD).toInt()*60*1000, this, SLOT(checkForEndedItems()));
}

void MainWindow::connectToDB() {
    sdb = QSqlDatabase::addDatabase("QSQLITE");
    sdb.setDatabaseName(qsettings->value("userSettings/dbName", qApp->applicationDirPath() + "/data.sqlite").toString());

    if(!sdb.open()) {
        QMessageBox msgBox;
        msgBox.setText("Не удаётся установить соединение с базой");
        msgBox.setDetailedText(sdb.lastError().text());
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();

        qDebug() << "Не удаётся установить соединение с базой";
        qDebug() << sdb.lastError().text();
        return;
    }

    setTableModel();
}

void MainWindow::setTableModel() {
    if(!sdb.tables().contains(DB_TABLE_NAME)) {
        sdb.exec("CREATE TABLE "+DB_TABLE_NAME+" ( 'chip' TEXT NOT NULL, 'map' TEXT NOT NULL, 'count' INTEGER, 'startTime' INTEGER NOT NULL, 'duration' INTEGER NOT NULL, 'endTime' INTEGER NOT NULL, 'device' TEXT NOT NULL, 'comment' TEXT, 'isDone' INTEGER DEFAULT 0, 'tester' TEXT NOT NULL, 'id' INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE )");
    }

    //if (tableModel) delete tableModel;
    tableModel = new QSqlTableModel(0, sdb);
}

void MainWindow::openSettingsWindowSlot() {
    emit openSettingsWindow();
}

void MainWindow::setFilter(int id) {
    dbFilter = id;
    reloadDataSlot();
}

void MainWindow::resetFilter() {
    dbFilter = (int) qsettings->value("userSettings/defaultFilter", 0).toInt();
    if (ui->filterBox->currentIndex() != dbFilter) reloadDataSlot();
    ui->filterBox->setCurrentIndex(dbFilter);

}

void MainWindow::export2CSV() {
    QFileDialog dialog(this);
    dialog.setNameFilter("Excel CSV file (*.csv)");
    dialog.setFileMode(QFileDialog::AnyFile);
    if (dialog.exec()) {
        QThread *thread  = new QThread();
        CSVexport *task = new CSVexport(tableModel, dialog.selectedFiles().at(0));
        task->moveToThread(thread);

        connect(task, SIGNAL(percent(int)), this, SLOT(exportProgress(int)));
        connect(thread, SIGNAL(started()), task, SLOT(exportData()));
        connect(task, SIGNAL(isDone(bool)), this, SLOT(exportFinishedStatus(bool)));

        thread->start();
    }

}

void MainWindow::printData() {
    printer = new Printer();
    printer->prepareHtmlSqlModel(tableModel);
    printer->printContent();
}

void MainWindow::saveData2Html() {
    printer = new Printer();
    printer->prepareHtmlSqlModel(tableModel);
    printer->saveContent2Html();
}

void MainWindow::immediateQuit() {
    qApp->quit();
}

void MainWindow::exportProgress(int progress) {
    ui->statusBar->showMessage("Экспорт данных в CSV формат. Завершено: "+QString::number(progress)+"%");
}

void MainWindow::exportFinishedStatus(bool status) {
    QMessageBox msgBox;

    if (status)
        msgBox.setText("Данные успешно экспортированыы");
    else
        msgBox.setText("Экспорт не удался.");



    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::copyRowSlot() {
    QSqlQuery q;
    QString query = "INSERT INTO " + DB_TABLE_NAME + "(chip, map, count, startTime, duration, endTime, device, comment, isDone, tester) SELECT chip, map, count, startTime, duration, endTime, device, comment, isDone, tester FROM " + DB_TABLE_NAME + " WHERE id ='" + QString::number(rightClickedIndex) + "'";
    if (q.exec(query)) {
        ui->statusBar->showMessage("Строка успешно скопирована");
        reloadDataSlot();
    } else {
        QMessageBox msgBox;
        msgBox.setText("Ошибка базы данных");
        msgBox.setDetailedText(q.lastError().text());
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();

        qDebug() << "Ошибка базы данных. copyRowSlot()";
        qDebug() << q.lastError().text();
    }}

void MainWindow::exportRowSlot() {
    QFileDialog dialog(this);
    dialog.setNameFilter("ES Tester dump files (*.esdmp)");
    dialog.setFileMode(QFileDialog::AnyFile);
    if(!dialog.exec()) return;

    QSqlQuery q;

    QString query = "SELECT * FROM " + DB_TABLE_NAME + " WHERE id =" + QString::number(rightClickedIndex);
    /*QSqlTableModel *tmpTableModel = new QSqlTableModel(0, sdb);
    tmpTableModel->setTable(DB_TABLE_NAME);
    tmpTableModel->setFilter(query);
    tmpTableModel->select();*/


    if (q.exec(query)) {

        QSqlRecord r;
        QStringList fields, values;
        QString fName = dialog.selectedFiles().at(0);
        QRegExp rx("*.esdmp");
        rx.setPatternSyntax(QRegExp::Wildcard);

        if(!rx.exactMatch(fName))
            fName.append(".esdmp");

        QFile *file = new QFile(fName);
        if(!(file->open(QIODevice::WriteOnly))) {
            QMessageBox msgBox;
            msgBox.setText("Не удалось открыть файл");
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
            return;
        }
        QTextStream ts(file);
        fields.clear();
        values.clear();

        while(q.next()) {
            r = q.record();
            for (uint i = 0; i < (uint) r.count()-1; i++) {
                fields.append(r.fieldName(i));
                values.append(r.value(i).toString());
            }
            ts << fields.join(IMPORT_EXPORT_DATA_SEPARATOR) + '\n';
            ts << values.join(IMPORT_EXPORT_DATA_SEPARATOR) + '\n';
        }
        file->close();
    } else {
        QMessageBox msgBox;
        msgBox.setText("Ошибка базы данных");
        msgBox.setDetailedText(q.lastError().text());
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();

        qDebug() << "Ошибка базы данных. exportRowSlot()";
        qDebug() << q.lastError().text();
    }


}
void MainWindow::importRowSlot() {
    QFileDialog dialog(this);
    dialog.setNameFilter("ES Tester dump files (*.esdmp)");
    dialog.setFileMode(QFileDialog::AnyFile);
    if(!dialog.exec()) return;

    QFile *file = new QFile(dialog.selectedFiles().at(0));
    if(!(file->open(QIODevice::ReadOnly))) {
        QMessageBox msgBox;
        msgBox.setText("Не удалось открыть файл");
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    QTextStream ts(file);
    QStringList fields, values;
    QSqlQuery q;
    // read fields
    QString tmpString = ts.readAll();
    QStringList lines = tmpString.split('\n');
    fields = lines.at(DUMP_IMPORT_HEADERS_LINE).split(IMPORT_EXPORT_DATA_SEPARATOR);
    // read values
    //tmpString = ts.readLine();
    QString tmpValues;
    tmpValues.clear();
    for (uint i = DUMP_IMPORT_DATA_LINE; i < lines.length(); i++)
        tmpValues.append(lines.at(i)).append("\n");
    values = tmpValues.split(IMPORT_EXPORT_DATA_SEPARATOR);
    file->close();

    // make a sql query
    tmpString = "INSERT INTO " + DB_TABLE_NAME + " (";
    for(uint i = 0; i < (uint) fields.count(); i++) {
        if (i > 0) tmpString.append(", ");
        tmpString.append(fields.at(i));
    }
    tmpString.append(") VALUES(");
    for (uint i = 0; i < (uint) values.count(); i++) {
        if (i > 0) tmpString.append(", ");
        tmpString.append("'" + values.at(i) + "'");
    }
    tmpString.append(");");
    if(!q.exec(tmpString)) {
        QMessageBox msgBox;
        msgBox.setText("Не удалось записать данные в базу");
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    ui->statusBar->showMessage("Данные успешно импортированы");
    reloadDataSlot();


}

void MainWindow::oneSecTimerSlot() {
    ui->currentDateTimeLabel->setText(QDateTime::currentDateTime().toString(CURRENT_DATE_FORMAT));
    // 1 sec Timer
    oneSecTimer.singleShot(500, this, SLOT(oneSecTimerSlot()));
}
