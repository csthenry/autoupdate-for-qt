#ifndef CHECKVERSION_H
#define CHECKVERSION_H

#include <QObject>
#include <QProcess>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include "globalval.h"

class GlobalVal;
class CheckVersion : public QObject
{
    Q_OBJECT
public:
    explicit CheckVersion(QObject *parent = nullptr,QString remoteUrl=nullptr);
    void requestRemoteVersion();

private:
    QString remoteInfoUrl;
    int readSysVersion();


private slots:
    void requestRemoteVersionFinished(QNetworkReply *reply);

signals:
    void sendMsg(QString msg);
    void upgradeBtnStatus(int status);

};

#endif // CHECKVERSION_H
