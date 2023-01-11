QT       += core gui
QT  += network
# QT  += gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

QMAKE_CXXFLAGS += /utf-8

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# zip解压所需库
INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib
include($$PWD/quazip/quazip.pri)
DEFINES += QT_DEPRECATED_WARNINGS QUAZIP_STATIC

SOURCES += \
    checkversion.cpp \
    download.cpp \
    globalval.cpp \
    handleziptype.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    checkversion.h \
    download.h \
    globalval.h \
    handleziptype.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    i18n/autoUpdater_zh_CN.ts \
    i18n/autoUpdater_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

RC_ICONS = resources/logo.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    autoUpdater.qrc \
    i18n/Language.qrc

DESTDIR = ./dist

# 申请管理员权限 此时需要使用管理员权限启动Qt Creator
QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"

#版本信息
VERSION = 1.0.1

#产品名称
QMAKE_TARGET_PRODUCT = "Automatic upgrade helper"

#文件说明
QMAKE_TARGET_DESCRIPTION = "QT Client Program Automatic Upgrade Tool"

#版权信息
QMAKE_TARGET_COPYRIGHT = "Copyright (c) 2023 henry bytecho.net"
#语言，简体中文
RC_LANG = 0x0804
