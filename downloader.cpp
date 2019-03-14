#include "downloader.h"

downloader::downloader(QObject *parent) : QObject(parent)
{
    QString url = "http://zhitenev.ru/wp-includes/js/jquery/jquery.js";
    QNetworkAccessManager manager;
    QNetworkReply *response = manager.get(QNetworkRequest(QUrl(url)));
    QEventLoop event;
    connect(response, SIGNAL(finished()), &event, SLOT(quit()));
    event.exec();
    QString content = response->readAll();
    qDebug() << content;
}
