#ifndef HANDLEZIPTYPE_H
#define HANDLEZIPTYPE_H

#include <QtNetwork>
//#include <QProgressDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QFile>
#include <QObject>
//#include <QDialog>
#include "QtGui/private/qzipreader_p.h" //zip解压
//#include "QtGui/private/qzipwriter_p.h" //zip压缩
#include <QEventLoop>
#include <QFileInfoList>
#include "globalval.h"

#include <memory>

QT_BEGIN_NAMESPACE
class QFile;
//class QSslError;
class QNetworkReply;
class GlobalVal;
QT_END_NAMESPACE

class HandleZipType : public QObject
{
    Q_OBJECT

public:
    HandleZipType(QObject *parent = nullptr);
    ~HandleZipType();
    void downloadZip(QUrl url);
    QString downloadProgress;

private:
     QUrl url;
     QNetworkAccessManager *manager;
     QNetworkReply *reply;
     std::unique_ptr<QFile> file;
     bool httpRequestAborted;
     std::unique_ptr<QFile> openFileForWrite(const QString &fileName);
     QString originFileName;
     QString zipFilePath;
     QString tempDir;

private slots:
    void startRequest(const QUrl &requestedUrl);
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
    void httpError();
    void networkReplyProgress(qint64 bytesRead, qint64 totalBytes);
    bool unzip(QString zipurl);
    bool cpdir(QString fromDir,QString toDir,bool f);

signals:
    void sendMsg(QString msg);
};

#endif // HANDLEZIPTYPE_H
