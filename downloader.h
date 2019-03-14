#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <qeventloop.h>

class downloader : public QObject
{
    Q_OBJECT
public:
    explicit downloader(QObject *parent = 0);

signals:

public slots:
};

#endif // DOWNLOADER_H
