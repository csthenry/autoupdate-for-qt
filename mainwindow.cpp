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

    //样式修改
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    //内层窗口添加对应的阴影效果
    QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setOffset(0, 0);
    shadow_effect->setColor(QColor(150,150,150));
    shadow_effect->setBlurRadius(20);
    ui->inner_widget->setGraphicsEffect(shadow_effect);
    //设置内层QWidget的边框和背景颜色
    ui->inner_widget->setStyleSheet("QWidget#inner_widget{border:1px solid #FFFFFF;border-radius:7px;background-color:#FFFFFF;}");

    QSettings *settings;
    settings = new QSettings(QDir::currentPath() + "/update/setting.ini",QSettings::IniFormat);
    QString upgradeRemoteUrl = settings->value("upgrade/url").toString();
    ui->chkUpgradeBtn->setVisible(false);   //隐藏不必要的检测更新btn
    if(upgradeRemoteUrl.isEmpty()){
        this->upgradeBtnReset(2);
        this->appendProgressMsg(tr("获取配置信息出错，请检查配置文件..."));
        this->appendProgressMsg(tr("程序将在%1秒后退出...").arg(5));
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
    //programRootDir.cdUp(); //更新程序根目录的上级目录
    deleteDir(QDir::currentPath() + "/update/");    //删除旧版本更新器文件
    GlobalVal::programRootDir = programRootDir.path();    //此处定义主程序根目录
    this->on_chkUpgradeBtn_clicked();
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
        clickPos = e->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton  //左键点击并且移动
            && e->pos().x() >= 0
            && e->pos().y() >= 0
            && e->pos().x() <= geometry().width()
            && e->pos().y() <= geometry().height() / 6) //范围在窗口的上面部分
    {
        move(e->pos() + pos() - clickPos);  //移动窗口
    }
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
                 if(url.fileName() == "updater.exe")
                     continue;  //新版本更新程序忽略更新
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
         this->appendProgressMsg(tr("[错误代码:500]参数校验失败..."));
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
    QString currDateTime =  QDateTime::currentDateTime().toString("hh:mm:ss");
    this->ui->progressMsgBox->append("["+currDateTime+"] " + msg);
}
/**
 * 槽函数：向textarea 文本框中追加数据
 * @brief MainWindow::receiveMsgDateln
 * @param msg
 */
void MainWindow::receiveMsgDate(QString msg){
    QString currDateTime =  QDateTime::currentDateTime().toString("hh:mm:ss");
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

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
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

void MainWindow::deleteDir(const QString &path)
{
    QDir tmpDir(path);
    foreach(QFileInfo fileInfo, tmpDir.entryInfoList(QDir::Files))
        if(fileInfo.fileName() != "version.dat" && fileInfo.fileName() != "setting.ini")
            tmpDir.remove(fileInfo.fileName());
    foreach(QString subDir, tmpDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        deleteDir(path + QDir::separator() + subDir); //递归删除子目录文件
        tmpDir.rmdir(subDir);    //删除文件夹
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

