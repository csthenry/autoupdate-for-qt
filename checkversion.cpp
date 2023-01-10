#include "checkversion.h"


#pragma execution_character_set("utf-8")

CheckVersion::CheckVersion(QObject *parent,QString remoteUrl)
    : QObject{parent}
{
    this->remoteInfoUrl = remoteUrl;
}
/**
 * 发起http请求，请求后端版本更新信息
 * @brief CheckVersion::requestRemoteVersion
 */
void CheckVersion::requestRemoteVersion(){
    QUrl url(this->remoteInfoUrl);
    emit sendMsg(tr("请求中..."));
    QNetworkRequest req;
    QNetworkAccessManager nam;
    connect(&nam,&QNetworkAccessManager::finished,this,&CheckVersion::requestRemoteVersionFinished);
    req.setUrl(url);
    QNetworkReply *reply = nam.get(req);
    QEventLoop loop;
    connect(reply,&QNetworkReply::finished,&loop,&QEventLoop::quit);
    loop.exec();
}
/**
 * 接收http请求响应数据
 * @brief requestRemoteVersionFinished
 * @param reply
 */
void CheckVersion::requestRemoteVersionFinished(QNetworkReply *reply){
    QString rsdata = reply->readAll();
    int sysVersion = this->readSysVersion();
    QJsonParseError jsonParseError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(rsdata.toUtf8(),&jsonParseError));
    if(QJsonParseError::NoError !=jsonParseError.error){
        emit sendMsg(tr("解析请求响应数据失败：%1").arg(jsonParseError.errorString()));
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();
    int newVer = jsonObj.value("version").toInt();
    if(newVer<=sysVersion){
        //不更新
        emit sendMsg(tr("当前版本已经是最新版本"));
        return;
    }
    emit upgradeBtnStatus(1);
    GlobalVal::newVersion = newVer;
    QString newVerStr = jsonObj.value("versionStr").toString();
    emit sendMsg(tr("发现新版本：%1，点击【立即更新】后开始更新").arg(newVerStr));
    int updateTtype = jsonObj.value("type").toInt();
    GlobalVal::updateTtype = updateTtype;
    QString zipurl = jsonObj.value("zipurl").toString();
    QJsonArray fileList = jsonObj.value("filelist").toArray();
    QString mainAppName = jsonObj.value("mainAppName").toString();
    GlobalVal::zipurl = zipurl;
    GlobalVal::fileList = fileList;
    GlobalVal::mainAppName = mainAppName;
}
/**
 * 获取本地程序版本号，版本号为纯数字
 * @brief CheckVersion::readSysVersion
 * @return
 */
int CheckVersion::readSysVersion()
{
    QString configFilePath = QDir::currentPath()+"/version.dat";
    configFilePath = QDir::toNativeSeparators(configFilePath);
    QFile writeFile(configFilePath);
    writeFile.open(QIODevice::ReadOnly);
    QDataStream in(&writeFile);
    int sysVersion;
    in >> sysVersion;
     writeFile.close();
     return sysVersion;
}
