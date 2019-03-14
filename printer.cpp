#include "printer.h"

Printer::Printer(QObject *parent) : QObject(parent)
{
    device = new QPrinter();
}

void Printer::prepareHtmlSqlModel(QSqlTableModel *model) {
    Pcontent.clear();
    Pcontent = "<html><head><meta charset='windows-2151'></head><body><style type='text/css'>table, tr, th, td {border: 1px solid #999; border-collapse: collapse; padding: 4px;} th {font-syle: bold;}</style><table>";


    for (int row = -1; row < model->rowCount(); row++) {
        Pcontent.append("    <tr>");
        for (int col = 0; col < model->columnCount(); col++) {
            if (col == TABLE_COL_ID) continue;
            if (row == -1) {
                Pcontent.append("        <th>" + model->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString() + "</th>");
            } else {
                Pcontent.append("        <td>" + model->data(model->index(row, col), Qt::DisplayRole).toString() + "</td>");
            }
        }
        Pcontent.append("    </tr>");
    }
    Pcontent.append("</table></body></html>");

}

void Printer::printContent() {
    QPrintDialog *dialog = new QPrintDialog(device);
    if (dialog->exec() == QDialog::Accepted) {
        QTextDocument doc;
        doc.setHtml(Pcontent);
        doc.print(device);
    }


    Pcontent.clear();
}

bool Printer::saveContent2Html() {
    QTextDocument doc;
    doc.setHtml(Pcontent);
    //doc
    QFileDialog dialog;
    dialog.setNameFilter("HTML files (*.html)");
    dialog.setFileMode(QFileDialog::AnyFile);
    if(dialog.exec())
        if(!dialog.selectedFiles().isEmpty()) {
            QString fileName = dialog.selectedFiles().at(0);

            QRegExp rx("*.html");
            rx.setPatternSyntax(QRegExp::Wildcard);

            if(!rx.exactMatch(fileName))
                fileName.append(".html");
            QFile file(fileName);
            if (file.open(QIODevice::ReadWrite)) {
                QTextStream stream(&file);
                stream << Pcontent << endl;
            }
            file.close();
        }
    return true;
}
