#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QEventLoop>
#include <QString>
#include <string>
#include <iostream>
#include <q2net.h>
#include <QFile>

using namespace std;

QByteArray urlretrieve(string url)
{
    QUrl qurl = QUrl(QString::fromStdString(url));
    QNetworkAccessManager manager;
    QNetworkReply *response = manager.get(QNetworkRequest(qurl));
    QEventLoop event;
    QObject::connect(response, SIGNAL(finished()), &event, SLOT(quit()));
    event.exec();
    QByteArray content = response->readAll();
    return content;
}


bool urlretrieve(string url, string file_name)
{
    QByteArray content = urlretrieve(url);
    QFile localFile(QString::fromStdString(file_name));
    if (!localFile.open(QIODevice::WriteOnly))
    {
        return 0;
    }
    localFile.write(content);
    localFile.close();

    return true;
}
