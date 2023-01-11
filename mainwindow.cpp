#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QThread>

#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSettings *settings;
    settings = new QSettings("setting.ini",QSettings::IniFormat);
    QString upgradeRemoteUrl = settings->value("upgrade/url").toString();
    ui->chkUpgradeBtn->setVisible(false);   //隐藏不必要的检测更新btn
    if(upgradeRemoteUrl.isEmpty()){
        this->upgradeBtnReset(2);
        this->appendProgressMsg(tr("获取配置信息出错，请检查应用程序是否正确...[错误代码:403]"));
        this->appendProgressMsg(tr("程序将在%1秒后退出。").arg(5));
        QTimer::singleShot(5000,qApp,SLOT(quit()));
        return;
    }
    cv = new CheckVersion(this->parent(),upgradeRemoteUrl);
    connect(cv,SIGNAL(sendMsg(QString)),this,SLOT(receiveMsgDateln(QString)));
    connect(cv,SIGNAL(upgradeBtnStatus(int)),this,SLOT(upgradeBtnReset(int)));
    download = new Download(this->parent());
    connect(download,SIGNAL(sendMsg(QString)),this,SLOT(receiveMsgDateln(QString)));
    handleZip = new HandleZipType(this->parent());
    connect(handleZip,SIGNAL(sendMsg(QString)),this,SLOT(receiveMsgDateln(QString)));

    /*#ifdef QT_NO_QDEBUG
        QDir thisDownDir = QDir::currentPath();
        thisDownDir.cdUp();
        GlobalVal::programRootDir = thisDownDir.absolutePath();
    #else
        GlobalVal::programRootDir = QDir::currentPath();
    #endif*/

    QDir programRootDir = QDir::currentPath();
    programRootDir.cdUp(); //更新程序根目录的上级目录
    GlobalVal::programRootDir = programRootDir.path();    //此处定义主程序根目录
    this->on_chkUpgradeBtn_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_chkUpgradeBtn_clicked()
{
    this->appendProgressMsg(tr("正在检测更新..."));
    cv->requestRemoteVersion();
}


void MainWindow::on_nowUpgradeBtn_clicked()
{
     this->upgradeBtnReset(2);
     int updateTtype = GlobalVal::updateTtype;
     QString programRootDir = GlobalVal::programRootDir;
    bool updateOK = false;
     if(updateTtype==1){
         QJsonArray fileList = GlobalVal::fileList;
         for(int i =0;i<fileList.size();i++){
             QJsonObject item = fileList[i].toObject();
             QString path = item.value("path").toString();
             QString downloadRootDir = programRootDir;
             if(!path.isEmpty() && path!="/"){
                downloadRootDir = programRootDir + "/" + path;
             }
             QJsonArray sublist = item.value("sublist").toArray();
             for(int j =0;j<sublist.size();j++){
                 QString fileUrl = sublist[j].toString();
                 fileUrl = Download::urlEncode(fileUrl);
                 QUrl url(fileUrl);
                 download->resetStatus();
                 download->downloadFile(url,downloadRootDir);
                 //this->syncVersion(); 由主程序更新版本文件
                 this->appendProgressMsg(tr("更新完成^_^"));
                 updateOK = true;
             }
         }
     }else if(updateTtype==2){
         QString zipurl = GlobalVal::zipurl;
         if(!zipurl.isEmpty()){
            QUrl url(zipurl);
            handleZip->downloadZip(url);
            //this->syncVersion(); 由主程序更新版本文件
            this->appendProgressMsg(tr("更新完成^_^"));
            updateOK = true;
         }
     }else{
         this->appendProgressMsg(tr("参数校验失败[错误代码:500]"));
     }
     //启动主程序
    if(updateOK){
        this->appendProgressMsg(tr("即将关闭更新程序，正在启动主程序..."));
        QTimer::singleShot(1500,this,&MainWindow::startMainApp);
        //this->startMainApp();
    }
}
/**
 * 向textarea 文本框中追加数据
 * @brief MainWindow::appendProgressMsg
 * @param msg
 */
void MainWindow::appendProgressMsg(QString msg){
    QString currDateTime =  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    this->ui->progressMsgBox->append("["+currDateTime+"] " + msg);
}
/**
 * 槽函数：向textarea 文本框中追加数据
 * @brief MainWindow::receiveMsgDateln
 * @param msg
 */
void MainWindow::receiveMsgDate(QString msg){
    QString currDateTime =  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    this->ui->progressMsgBox->append("["+currDateTime+"] "+msg);
}
/**
 * 槽函数：向textarea 文本框中追加数据
 * @brief MainWindow::receiveMsgDateln
 * @param msg
 */
void MainWindow::receiveMsgDateln(QString msg){
    this->appendProgressMsg(msg);
}
/**
 * 修改按钮状态
 * @brief MainWindow::upgradeBtnReset
 * @param status 0:初始状态，1有更新，2更新中
 */
void MainWindow::upgradeBtnReset(int status){
    GlobalVal::status = status;
    switch(status){
    case 0:
        this->ui->chkUpgradeBtn->setEnabled(true);
        this->ui->nowUpgradeBtn->setEnabled(false);
        break;
    case 1:
        this->ui->chkUpgradeBtn->setEnabled(false);
        this->ui->nowUpgradeBtn->setEnabled(true);
        break;
    case 2:
        this->ui->chkUpgradeBtn->setEnabled(false);
        this->ui->nowUpgradeBtn->setEnabled(false);
        break;
    }
}
/**
 * 同步最新版本号到本地版本文件
 * @brief MainWindow::syncVersion
 */
void MainWindow::syncVersion()
{
    QString configFilePath = QDir::currentPath()+"/version.dat";
    configFilePath = QDir::toNativeSeparators(configFilePath);
    QFile writeFile(configFilePath);
    writeFile.open(QIODevice::WriteOnly);
    QDataStream out(&writeFile);
    out << GlobalVal::newVersion;
    writeFile.close();
}
/**
 * 关闭本程序，启动主程序
 * @brief MainWindow::startMainApp
 */
void MainWindow::startMainApp(){
    QString mainAppName = GlobalVal::mainAppName;
    qApp->quit();
    if(!mainAppName.isEmpty()){
        QProcess::startDetached(GlobalVal::programRootDir+"/"+mainAppName,QStringList(),GlobalVal::programRootDir);
    }
}

/**
 * 窗口关闭处理,弹出确认提示
 * @brief BrowserWindow::closeEvent
 * @param event
 */
/*
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(GlobalVal::status==2 || GlobalVal::status==0){
        event->accept();
        deleteLater();
        return;
    }
    QMessageBox box(QMessageBox::Warning,tr("提示"),tr("确定要关闭吗？"));
        box.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        box.setButtonText(QMessageBox::Ok,QString(tr("确定")));
        box.setButtonText(QMessageBox::Cancel,QString(tr("取消")));
    int res = box.exec ();
    if (res == QMessageBox::Cancel) {
        event->ignore();
        return;
    }
    event->accept();
    deleteLater();
}
*/

