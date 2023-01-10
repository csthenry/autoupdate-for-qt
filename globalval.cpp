#include "globalval.h"
#include <QDir>

#pragma execution_character_set("utf-8")

int GlobalVal::newVersion;
QString  GlobalVal::programRootDir = QDir::currentPath();
QString GlobalVal::zipurl = "";
QJsonArray GlobalVal::fileList;
int GlobalVal::updateTtype;
QString GlobalVal::mainAppName = "";
/**
 *
 * @brief GlobalVal::status
 */
int GlobalVal::status = 0;
