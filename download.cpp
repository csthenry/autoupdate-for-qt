#include "download.h"
#include <QtNetwork>
#include <QUrl>
#include <QTextCodec>

#pragma execution_character_set("utf-8")

Download::Download(QObject *parent)
    : QObject(parent)
    , reply(nullptr)
    , file(nullptr)
    , httpRequestAborted(false)
{
     manager = new QNetworkAccessManager(this);
}

Download::~Download()
{
}

void Download::downloadFile(QUrl newUrl,QString downloadDirectory)
{
    if (!newUrl.isValid()) {
        emit sendMsg(tr("下载地址[%1]校验失败。").arg(newUrl.toString()));
        return;
    }

    QString fileName = newUrl.fileName();
    this->originFileName = fileName;
    emit sendMsg(tr("开始下载文件：%1").arg(fileName));
    if (fileName.isEmpty()){

    }
    if(downloadDirectory.isEmpty()){
        downloadDirectory = QDir::currentPath();
    }
    bool useDirectory = !downloadDirectory.isEmpty() && QFileInfo(downloadDirectory).isDir();
    if (useDirectory){
        fileName.prepend(downloadDirectory + '/');
    }
    if (QFile::exists(fileName)) {
        QFile::remove(fileName);
    }
    file = openFileForWrite(fileName);
    if (!file)
        return;
    // schedule the request
    startRequest(newUrl);
}

void Download::startRequest(const QUrl &requestedUrl)
{
    url = requestedUrl;
    httpRequestAborted = false;

    QNetworkRequest request(url);
    reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, &Download::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &Download::httpReadyRead);

    connect(reply, &QNetworkReply::errorOccurred, this, &Download::httpError);

    connect(reply, &QNetworkReply::downloadProgress, this, &Download::networkReplyProgress);
    QEventLoop loop;
    connect(reply,&QNetworkReply::finished,&loop,&QEventLoop::quit);
    loop.exec();

}

void Download::cancelDownload()
{
    httpRequestAborted = true;
    reply->abort();
}

void Download::httpFinished(){
    QFileInfo fi;
    if (file) {
        fi.setFile(file->fileName());
        file->close();
        file.reset();
    }

    if (httpRequestAborted) {
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    if (reply->error()) {
        QFile::remove(fi.absoluteFilePath());
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    reply->deleteLater();
    reply = nullptr;

    if (!redirectionTarget.isNull()) {
        const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
        startRequest(redirectedUrl);
        return;
    }
}

void Download::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file){
        file->write(reply->readAll());
    }
}

void Download::httpError(){
     emit sendMsg(tr("文件[%1]下载失败").arg(this->originFileName));
}

std::unique_ptr<QFile> Download::openFileForWrite(const QString &fileName)
{
    std::unique_ptr<QFile> file(new QFile(fileName));
    if (!file->open(QIODevice::WriteOnly)) {
        return nullptr;
    }
    return file;
}

void Download::networkReplyProgress(qint64 bytesRead, qint64 totalBytes)
{

    QString totalstr;
    QString readstr;
    QRegExp reg;
    reg.setPattern("(\\.){0,1}0+$");
    if(totalBytes>=1048576){
        double totalMb =  totalBytes/1024/1024;
        totalstr = QString("%1").arg(totalMb,0,'f',3).replace(reg,"") + "MB";
    }else{
        double totalKb =  totalBytes/1024;
        totalstr = QString("%1").arg(totalKb,0,'f',3).replace(reg,"") + "KB";
    }
    if(bytesRead>=1048576){
        double readMb =  bytesRead/1024/1024;
        readstr = QString("%1").arg(readMb,0,'f',3).replace(reg,"") + "MB";
    }else{
        double readKb =  bytesRead/1024;
        readstr = QString("%1").arg(readKb,0,'f',3).replace(reg,"") + "KB";
    }
    QString thisProgres = readstr + "/" + totalstr;
    emit sendMsg(tr("文件[%1]下载进度：%2").arg(this->originFileName,thisProgres));
    if(bytesRead==totalBytes){
       emit sendMsg(tr("文件[%1]下载完成。").arg(this->originFileName));
    }
}

void Download::resetStatus(){
    this->reply = nullptr;
    this->file = nullptr;
    this->httpRequestAborted = false;
    this->originFileName ="";
}
/**
 * URL编码,避免中文乱码
 * @brief Download::urlEncode
 * @param url
 * @return
 */
QString Download::urlEncode(QString url){
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
//    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
//    QByteArray byteArr =  codec->fromUnicode(url.toUtf8());//url.toUtf8();
    QByteArray byteArrPercentEncoded =  QUrl::toPercentEncoding(url.toLocal8Bit(),".:/-");
    return QString(byteArrPercentEncoded);
}
