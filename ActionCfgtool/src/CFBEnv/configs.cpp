#include "stdafx.h"
#include "configs.h"
#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>
#include <QProcessEnvironment>
#include <QSettings>
#include <QLocale>
#include <QDomDocument>
#include <QDomElement>
#include "filedirhandle.h"
#include "fileutils.h"
#include "constant.h"
#include "robotdef.h"
#include "singleton.hpp"

IMPL_SINGLETON_CLASS(CConfigs);

CConfigs::CConfigs()
{
    createDirsForApp();
}

CConfigs::~CConfigs()
{

}

void CConfigs::createDirsForApp()
{
    CFileDirHandle::mkpath(CConfigs::getLocalActLibDir());
    CFileDirHandle::mkpath(CConfigs::getLocalUserConfigDir());
    CFileDirHandle::mkpath(CConfigs::getLocalCusTemplateDir());
    CFileDirHandle::mkpath(CConfigs::getLocalLogDir());
    CFileDirHandle::mkpath(CConfigs::getLocalMusicDir());
    CFileDirHandle::mkpath(CConfigs::getLocalPreviewImgDir());
    CFileDirHandle::mkpath(CConfigs::getLocalDetailedImageDir());
    CFileDirHandle::mkpath(CConfigs::getLocalRecommendImgDir());
    CFileDirHandle::mkpath(CConfigs::getLocalTempDir());
    CFileDirHandle::mkpath(CConfigs::getLocalWaveDir());
}

QString CConfigs::getFilePath(const QString &fileDir, const QString &fileName)
{
    return CFileDirHandle::getFilePath(fileDir, fileName);
}

QString CConfigs::getAppBasicDataDir()
{
#ifdef Q_OS_MAC
    QString appBasicDataDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
#else
    QString appBasicDataDir = QCoreApplication::applicationDirPath();
#endif

    return QDir::toNativeSeparators(appBasicDataDir);
}

QString CConfigs::getAppBasicInstallDir()
{
    QString applicationDir = QCoreApplication::applicationDirPath();

#ifdef Q_OS_MAC
    applicationDir = getFilePath(applicationDir, "/.."); //APP exc is at macos dir
#else
    //
#endif

    return QDir::toNativeSeparators(applicationDir);
}

QString CConfigs::getLocalPreviewImgDir()
{
    QString appBasicDataDir = getAppBasicDataDir();
    return getFilePath(appBasicDataDir, PREVIEW_IMAGE_DIR_NAME);
}

QString CConfigs::getLocalPreviewImgPath(const QString &fileBasicName)
{
    return getFilePath(getLocalPreviewImgDir(), fileBasicName);
}

QString CConfigs::getLocalDetailedImageDir()
{
    QString appBasicDataDir = getAppBasicDataDir();
    return getFilePath(appBasicDataDir, DETAILED_IMAGE_DIR_NAME);
}

QString CConfigs::getLocalDetailedImagePath(const QString& fileBasicName)
{
    return getFilePath(getLocalDetailedImageDir(), fileBasicName);
}

QString CConfigs::getLocalRecommendImgDir()
{
    QString appBasicDataDir = getAppBasicDataDir();
    return getFilePath(appBasicDataDir, RECOMMEND_IMAGE_DIR_NAME);
}

QString CConfigs::getLocalRecommendImgPath(const QString &fileBasicName)
{
    return getFilePath(getLocalRecommendImgDir(), fileBasicName);
}

QString CConfigs::getLocalTempDir()
{
    QString appBasicDataDir = getAppBasicDataDir();
    return getFilePath(appBasicDataDir, TEMP_DIR_NAME);
}

QString CConfigs::getLocalTempPath(const QString &fileBasicName)
{
    return getFilePath(getLocalTempDir(), fileBasicName);
}

QString CConfigs::getLocalWaveDir()
{
    QString appBasicDataDir = getAppBasicDataDir();
    return getFilePath(appBasicDataDir, WAVE_DIR_NAME);
}

QString CConfigs::getLocalWavePath(const QString &fileBasicName)
{
    return getFilePath(getLocalWaveDir(), fileBasicName);
}

QString CConfigs::getLocalDownloadDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
}

QString CConfigs::getLocalDownloadPath(const QString &fileName)
{
    return getFilePath(getLocalDownloadDir(), fileName);
}

QString CConfigs::getLocalActLibDir()
{
    QString appBasicDataDir = getAppBasicDataDir();
    return getFilePath(appBasicDataDir, ACT_LIB_DIR_NAME);
}

QString CConfigs::getLocalActLibPath(const QString &fileBasicName)
{
    return getFilePath(getLocalActLibDir(), fileBasicName);
}

QString CConfigs::getFileNameFromAppLocalDataLocationPath(const QString & fileName)
{
    QString qsAppLocalDataLocationPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
//    if(fileName[0] != '/')
//    {
//        qsAppLocalDataLocationPath.append('/');
//    }
    if (!fileName.startsWith("/") || !fileName.startsWith("\\")) {
        qsAppLocalDataLocationPath.append('/');
    }
    QString qsFilePathName = QDir::toNativeSeparators(qsAppLocalDataLocationPath + fileName);
    if(FileUtils::exists(qsFilePathName)) {
        FileUtils::createDir(qsFilePathName);
    }
    return qsFilePathName;
}

QString CConfigs::getLocalUserConfigDir()
{
    //this is for user configs
    QString appBasicDataDir = getAppBasicDataDir();
    return getFilePath(appBasicDataDir, CONFIG_DIR_NAME);
}

QString CConfigs::getLocalUserConfigPath(const QString &fileBasicName)
{
    return getFilePath(getLocalUserConfigDir(), fileBasicName);
}

QString CConfigs::getLocalCusTemplateDir()
{
    //!!!it is data dir
    QString appBasicDataDir = getAppBasicDataDir();
    return getFilePath(appBasicDataDir, CUS_TEMPLATE_DIR_NAME);
}

QString CConfigs::getLocalDefaultTemplateDir()
{
   //!!!this is app package install dir
    QString appBasicInstallDir = getAppBasicInstallDir();
    return getFilePath(appBasicInstallDir, DEFAULT_TEMPLATE_DIR_NAME);
}

QString CConfigs::getLocalLogDir()
{
    QString appBasicDataDir = getAppBasicDataDir();
    return getFilePath(appBasicDataDir, LOG_DIR_NAME);
}

QString CConfigs::getLocalLogFilePath()
{
    return getFilePath(getLocalLogDir(), "AlphaRobot.log");
}

QString CConfigs::getLocalMusicDir()
{
    QString appBasicDataDir = getAppBasicDataDir();
    return getFilePath(appBasicDataDir, MUSIC_DIR_NAME);
}

QString CConfigs::getLocalMusicPath(const QString &fileBasicName)
{
    return getFilePath(getLocalMusicDir(), fileBasicName);
}

QString CConfigs::getLocalLanguageDir()
{
    QString strLanguageDir;

#ifdef Q_OS_WIN
    strLanguageDir = getFilePath(CConfigs::getAppBasicInstallDir(), LANGUAGES_DIR_NAME);
#else
    strLanguageDir = CConfigs::getLocalResourcesDir();
#endif

    return strLanguageDir;
}

QString CConfigs::getLocalResDir()
{
    return getFilePath(CConfigs::getAppBasicInstallDir(), RES_DIR_NAME);
}

QString CConfigs::getLocalResPath(const QString &fileBasicName)
{
    return getFilePath(getLocalResDir(), fileBasicName);
}

QString CConfigs::getLocalResourcesDir()
{
    return getFilePath(CConfigs::getAppBasicInstallDir(), RESOURCES_DIR_NAME);
}

QString CConfigs::getLocalResourcesPath(const QString &fileBasicName)
{
    return getFilePath(getLocalResourcesDir(), fileBasicName);
}

QString CConfigs::getLocalRobotDir()
{
    return getFilePath(CConfigs::getAppBasicInstallDir(), ROBOT_DIR_NAME);
}

QString CConfigs::getLocalRobotPath(const QString &fileBasicName)
{
    return getFilePath(getLocalRobotDir(), fileBasicName);
}


QString CConfigs::getDownloadPath()
{
    return getFilePath(CConfigs::getAppBasicDataDir(), DOWNLOAD_DIR);
}

QString CConfigs::getIniConfigFile()
{
    return getLocalUserConfigPath("config.ini");
}


QString CConfigs::getUpgradeFilePath()
{
    const QString appBasicDataDir = getAppBasicDataDir();
    return getFilePath(appBasicDataDir, UPGRADE_XML_NAME);
}

QMap<int, QString> CConfigs::getLocalTemplateFullPath(const emTreeModelType& type)
{
    QString appBasicDataDir = getAppBasicDataDir() + QDir::separator() + TEMPLATE_DIR;
    QMap<int, QString> mapT;
    if (type == TREEMODEL_DEFAULT_TYPE)
    {
        //FlowAction监控的文件夹为FlowTemplate
        mapT.insert(FLOW_TEMPLATE_ACTION, getFilePath(appBasicDataDir, TEMPLATE_NODE_DIR_NAME[CFB_DEFAULT_FLOW_TEMPLATE]));
        // mapT.insert(TEMPLATE_NODE_TYPE_ACTION, getFilePath(appBasicDataDir, TEMPLATE_NODE_DIR_NAME[CFB_DEFAULT_ACTION_TEMPLATE]));
        QString qsActionPath = getFilePath(appBasicDataDir, TEMPLATE_NODE_DIR_NAME[CFB_DEFAULT_ACTION_TEMPLATE]);
        for (int i = ACTION_TEMPLATE_EAR; i < ACTION_TEMPLATE_END; ++i)
        {
            mapT.insert(i, getFilePath(qsActionPath, TEMPLATE_ACTION_NODE_DIR_NAME[i-ACTION_TEMPLATE_EAR]));
        }
    }
    else if(type == TREEMODEL_CUSTOM_TYPE)
    {
        //FlowAction监控的文件夹为FlowTemplate
        mapT.insert(FLOW_TEMPLATE_ACTION, getFilePath(appBasicDataDir, TEMPLATE_NODE_DIR_NAME[CFB_CUSTOM_FLOW_TEMPLATE]));
        //mapT.insert(TEMPLATE_NODE_TYPE_ACTION, getFilePath(appBasicDataDir, TEMPLATE_NODE_DIR_NAME[CFB_CUSTOM_ACTION_TEMPLATE]));
        QString qsActionPath = getFilePath(appBasicDataDir, TEMPLATE_NODE_DIR_NAME[CFB_CUSTOM_ACTION_TEMPLATE]);
        for (int i = ACTION_TEMPLATE_EAR; i < ACTION_TEMPLATE_END; ++i)
        {
            mapT.insert( i, getFilePath(qsActionPath, TEMPLATE_ACTION_NODE_DIR_NAME[i-ACTION_TEMPLATE_EAR]));
        }
    }
    return mapT;
}

QString CConfigs::getLocalTemplateDir()
{
    return getFilePath(CConfigs::getAppBasicDataDir(), TEMPLATE_DIR);
}


QString CConfigs::getSystemAppDataDir()
{
    QString value;
#ifdef Q_OS_WIN
    value = QDir::toNativeSeparators(QProcessEnvironment::systemEnvironment().value("APPDATA") + QDir::separator() + getAppName());
#else
    //leo unfinished need add mac appdata path
    // value = QDir::toNativeSeparators(GetFileNameFromAppLocalDataLocationPath("/"));
#endif
    return value;
}


QString CConfigs::getAppName()
{
    QString appPath = QDir::toNativeSeparators(qApp->applicationFilePath());
    QFileInfo info(appPath);
    QString fileName = info.fileName();
    return fileName.left(fileName.lastIndexOf("."));
}


QString CConfigs::getAppTempDataDir(const QString& qsName)
{
    //获取软件临时
    QString path =  QDir::toNativeSeparators(getSystemAppDataDir() + QDir::separator() + qsName);
    if (!CFileDirHandle::isDirExist(path))
        CFileDirHandle::mkpath(path);
    return path;
}


QString CConfigs::getUserDirPath(const QString &strDirName)
{
    QString strPath;
#ifdef Q_OS_MAC
    strPath = QDir::toNativeSeparators(getFileNameFromAppLocalDataLocationPath("/" + strDirName));
#else
    strPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/" + strDirName);
#endif
    return strPath;
}

QString CConfigs::getUserLanguage()
{
    QString strConfigFile = CConfigs::getLocalUserConfigPath("settings.ini");

    QSettings settings(strConfigFile, QSettings::IniFormat);
    settings.beginGroup(LANGUAGE_SETTING_WIDGET_GROUP);
    QString strLanguageKeyword = settings.value(LANGUAGE_KEYWORD).toString();
    // 如果配置文件没有记录语言信息，则使用系统语言
    if (strLanguageKeyword.isEmpty())
    {
        strLanguageKeyword = QLocale::system().name();
    }
    if (strLanguageKeyword.compare("zh_CN", Qt::CaseInsensitive) != 0)
    {
        strLanguageKeyword = "en";
    }
    return strLanguageKeyword;
}

QString CConfigs::getAppSubDirPath(const QString &strDirName)
{
    QString strPath;
#ifdef Q_OS_MAC
    strPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/../" + strDirName);
#else
    strPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/" + strDirName);
#endif
    return strPath;
}


void CConfigs::readModelXml(const QDomElement& element, std::vector<MOTOR_DATA *> &motorlist)
{
    QDomNode node = element.firstChild();
    static MOTOR_DATA* pMotorData = NULL;

    while (!node.isNull())
    {
        QDomElement nodeelem = node.toElement();
        if (nodeelem.tagName() == "Motor")
        {
            pMotorData = new MOTOR_DATA;
            motorlist.push_back(pMotorData);
            readModelXml(nodeelem, motorlist);
        }
        else if (nodeelem.tagName() == "ID")
        {
            QDomNode childnode = node.firstChild();
            if (childnode.nodeType() == QDomNode::TextNode)
            {
                if (pMotorData)
                    pMotorData->nID = childnode.toText().data().toInt();
            }
        }
        else if (nodeelem.tagName() == "InitAngle")
        {
            QDomNode childnode = node.firstChild();
            if (childnode.nodeType() == QDomNode::TextNode)
            {
                if (pMotorData)
                    pMotorData->nAngle = childnode.toText().data().toInt();
            }
        }

        node = node.nextSibling();
    }
}


void CConfigs::readModelXml(const QString& sXmlFilePath, vector<MOTOR_DATA *> &motorlist)
{
    QFile file(sXmlFilePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QDomDocument doc;
        doc.setContent(&file);
        readModelXml(doc.documentElement(), motorlist);
        file.close();
    }
}


int CConfigs::getCfgMotorCount()
{
    vector<MOTOR_DATA*> motorlist;
    QString sXmlPath = getAppSubDirPath(SUBDIR_ROBOTMODEL) + QDir::separator() + MODEL_XML_NAME;
    readModelXml(sXmlPath, motorlist);
    return motorlist.size();
}


const QString CConfigs::getFileNameFromOuputPath(QString const & strFileName)
{
    QString strAppcationDirPath = QCoreApplication::applicationDirPath();
    if (strFileName[0] != '/')
    {
        strAppcationDirPath.append('/');
    }
    return QDir::toNativeSeparators(strAppcationDirPath + strFileName);
}


void CConfigs::loadConfig()
{

}


const QString CConfigs::getDumpPath()
{
    const QString appBasicDataDir = getAppBasicDataDir();
    return getFilePath(appBasicDataDir, DUMP_DIR_NAME);
}

QString CConfigs::getUpgradeExeFullPath()
{
    const QString appBasicDataDir = getAppBasicDataDir();
    return getFilePath(appBasicDataDir, UPGRADE_EXE_NAME);
}
