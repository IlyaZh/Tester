#include "csvexport.h"

CSVexport::CSVexport(QSqlTableModel *mod, QString _filename)
{
    tModel = mod;
    fileName = _filename;
}

void CSVexport::exportData() {
    QRegExp rx("*.csv");
    rx.setPatternSyntax(QRegExp::Wildcard);

    if(!rx.exactMatch(fileName))
        fileName.append(".csv");

    QFile *file = new QFile(fileName);
    if (file->open(QIODevice::WriteOnly)) {
        QTextStream stream(file);
        QStringList strList;
        rows = tModel->rowCount();
        rowsDone = 0;
        progress = 0;
        // add header
        strList.clear();
        for (uint j = 0; j < tModel->columnCount(); j++)
            strList.append(tModel->headerData(j, Qt::Horizontal).toString());
        stream << strList.join(";") + "\n";
        // end of adding of header


        for (uint i = 0; i < rows; i++) {
            strList.clear();
            for (uint j = 0; j < tModel->columnCount(); j++)
                strList.append(tModel->data(tModel->index(i, j)).toString());

            stream << strList.join(";") + "\n";
            rowsDone += 1;
            progress = rowsDone*100/rows;
            // do action every 2 percents of all tasks
            if (!(progress % 2)) {
                emit percent(progress);
            }
        }
        file->close();
        emit isDone(true);
    } else {
        emit isDone(false);
    }
}
