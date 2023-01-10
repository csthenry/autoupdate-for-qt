#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QTextCodec>
#include <QObject>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("autoUpdater");
    QCoreApplication::setOrganizationDomain("hlinfo.net");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    //支持老显卡设置
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QGuiApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    QApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(QStringLiteral("updateLogo")));
//    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    //设置中文编码
   #if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
       #if _MSC_VER
           QTextCodec *codec = QTextCodec::codecForName("GBK");
       #else
           QTextCodec *codec = QTextCodec::codecForName("UTF-8");
       #endif
       QTextCodec::setCodecForLocale(codec);
       QTextCodec::setCodecForCStrings(codec);
       QTextCodec::setCodecForTr(codec);
   #else
       QTextCodec *codec = QTextCodec::codecForName("UTF-8");
       QTextCodec::setCodecForLocale(codec);
   #endif
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "autoUpdate_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    //固定大小
    w.setFixedSize(w.width(),w.height());
    //隐藏最大化最小化按钮
    w.setWindowFlags(w.windowFlags() &~Qt::WindowMinMaxButtonsHint);
    w.setWindowTitle(QObject::tr("WePlanet 在线更新"));
    w.show();
    return a.exec();
}
