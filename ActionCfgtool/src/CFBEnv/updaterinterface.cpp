#include "stdafx.h"
#include "updaterinterface.h"
#include "configs.h"
#include "iniconfig.h"
#include "fileutils.h"
#include <algorithm>
#include <QDomDocument>
#include <QDebug>
#include <QProcess>
#include "httpclient.h"

UpdaterInterface::UpdaterInterface() 
{
    m_qsNativeVersion = CIniConfig::instance()->getVersion();
    m_qsBaseUrl = CIniConfig::instance()->getBaseUpgradeUrl();
}


UpdaterInterface::~UpdaterInterface()
{

}


QString UpdaterInterface::formatVersionStr(const QString & in)
{
    if (in.isEmpty()) {
        return "";
    }
    QString out = in;
    foreach (QChar c, in) {
        if (!c.isNumber()) {
            out.remove(c, Qt::CaseInsensitive);
        }
    }
    return out;
}


bool UpdaterInterface::isUpgrading(const QString & qsNativeVersion, const QString & qsRemoteVersion)
{
    if (qsRemoteVersion.isEmpty()) {
        return false;
    }
    if (qsNativeVersion.isEmpty()) {
        return true;
    }
    int native = formatVersionStr(qsNativeVersion).toUInt();
    int remote = formatVersionStr(qsRemoteVersion).toUInt();
    return remote > native;
}


bool UpdaterInterface::upgradingVersionXmlFile(const QString &qsXmlFullPath, VERSIONINFO *pInfo)
{
    if (qsXmlFullPath.isEmpty() || pInfo == nullptr) {
        return false;
    }
    else {
        if (FileUtils::exists(qsXmlFullPath)) {
            try {
                FileUtils::deleteFile(qsXmlFullPath);
            }
            catch (...) {
                
            }
        }
        FileUtils::createFile(qsXmlFullPath);
        const QString qsBaseDir = CConfigs::getAppBasicDataDir();
        QFileInfoList fileInfoList = FileUtils::getFileInfoList(qsBaseDir);
        QFile file(qsXmlFullPath);
        if (!file.open(QFile::WriteOnly | QFile::Truncate)) 
            return false;

        pInfo->_mainVersion = m_qsNativeVersion;
        pInfo->_url = m_qsBaseUrl;

        QDomDocument doc;
        QDomProcessingInstruction instruction;
        instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
        doc.appendChild(instruction);
        //添加根节点  
        QDomElement root = doc.createElement("AutoUpdater");
        root.setAttribute("version", m_qsNativeVersion);
        doc.appendChild(root);
        int index = 0;
        foreach (QFileInfo info , fileInfoList) {
            QString qs = info.filePath();
            QString fileName = info.fileName();
            if (fileName == "." || fileName == "..") {
                continue;
            }

            QString qsName = FileUtils::getFileName(qs);
            QString qsBaseName = FileUtils::getBaseName(qs);
            QString qsMd5 = FileUtils::getMd5(qs);
            QString qsUrl = m_qsBaseUrl + "/" + qsName;

            ProgramFileInfo programFileInfo;
            programFileInfo._path = qs;
            programFileInfo._md5 = qsMd5;
            programFileInfo._url = qsUrl;
            pInfo->_files_map.insert(std::make_pair(qsName, programFileInfo));

            QDomText text; //设置括号标签中间的值  
            QDomElement item = doc.createElement("File");
            item.setAttribute("id", ++index);
            item.setAttribute("name", qsBaseName);
            QDomElement name = doc.createElement("name");
            text = doc.createTextNode(qsName);
            name.appendChild(text);
            item.appendChild(name);

            QDomElement md5 = doc.createElement("md5");
            text = doc.createTextNode(qsMd5);
            md5.appendChild(text);
            item.appendChild(md5);

            QDomElement url = doc.createElement("url");
            text = doc.createTextNode(qsUrl);
            url.appendChild(text);
            item.appendChild(url);

            QDomElement path = doc.createElement("path"); //创建子元素
            text = doc.createTextNode(qs);
            path.appendChild(text);
            item.appendChild(path);

            root.appendChild(item);
        }

        //输出到文件  
        QTextStream out_stream(&file);
        doc.save(out_stream, 4); //缩进4格  
        file.close();
    }
    return true;
}


bool UpdaterInterface::parseVersionXmlFile(const QString &qsXmlPath, VERSIONINFO *pInfo)
{
    if (pInfo == nullptr) {
        return false;
    }
    QFile file(qsXmlPath);
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }
    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }
    file.close();
    QString qsDownloadPath = CConfigs::getLocalDownloadDir();
    QDomElement root = doc.documentElement();
    pInfo->_mainVersion = root.attribute("version");
    QDomNode node = root.firstChild();
    while (!node.isNull()) {
        if (node.isElement()) {
            QDomElement e = node.toElement();
            if (e.nodeName().compare("File", Qt::CaseInsensitive) == 0) {
                QDomNodeList list = e.childNodes();
                ProgramFileInfo programFileInfo;
                QString qsName;
                for (int i = 0; i < list.count(); i++) {
                    QDomNode n = list.at(i);
                    QString qsNodeName = n.nodeName();
                    if (n.isElement() && qsNodeName.compare("name", Qt::CaseInsensitive) == 0 ) {
                        qsName = n.toElement().text();
                        if (qsName.isEmpty()) {
                            return false;
                        }
                    }
                    else if (n.isElement() && qsNodeName.compare("md5", Qt::CaseInsensitive) == 0) {
                        programFileInfo._md5 = n.toElement().text();
                        if (programFileInfo._md5.isEmpty()) {
                            return false;
                        }
                    }
                    else if (n.isElement() && qsNodeName.compare("url", Qt::CaseInsensitive) == 0) {
                        programFileInfo._url = n.toElement().text();
                        if (programFileInfo._url.isEmpty()) {
                            return false;
                        }
                    }
                    else if (n.isElement() && qsNodeName.compare("path", Qt::CaseInsensitive) == 0) {
                        programFileInfo._path = n.toElement().text();
                    }
                }
                if (programFileInfo._path.trimmed().isEmpty()) {
                    programFileInfo._path = qsDownloadPath + "/" + qsName;
                }
                pInfo->_files_map.insert(std::make_pair(qsName, programFileInfo));
            }
        }
        node = node.nextSibling();
    }
    return true;
}


bool UpdaterInterface::checkUpgradeAvailable()
{
    return isUpgrading(m_qsNativeVersion, m_qsRemoteVersion);
}


void UpdaterInterface::checkUpdate()
{
    if (checkUpgradeAvailable()) {
        startUpgrading();
    }
}


void UpdaterInterface::startUpgrading()
{
    const QString qsAutoUpgratePath = "";
    //QProcess *upgradeProcess = new QProcess;
    //upgradeProcess->start("E:\\Alpha2\\Alpha2_QT_Mini\\AlphaRobot2\\AlphaRobot\\Win32\\Debug\\Updater.exe");
}


bool UpdaterInterface::calculatingPatch(const VERSIONINFO *native, const VERSIONINFO *remote, list<ProgramFileInfo>& patch)
{
    patch.clear();
    if (native == nullptr || remote == nullptr) {
        return false;
    }
    const map<QString, ProgramFileInfo> &nativeData = native->_files_map;
    const map<QString, ProgramFileInfo> &remoteData = remote->_files_map;
    for (auto itRemote = remoteData.begin(); itRemote != remoteData.end(); ++itRemote) {
        QString name = itRemote->first;
        if (name.length() < 2) {
            continue;
        }
        auto itNative = nativeData.find(name);
        // 重置更新路径
        ProgramFileInfo info = itRemote->second;
        if (itNative != nativeData.end()) {
            if (itNative->second._md5 != itRemote->second._md5) {
                info._path = itNative->second._path;
                patch.push_back(info);
            }
        }
        // 新增文件
        else {
            info._path = itNative->second._path;
            patch.push_back(info);
        }
    }
    //for test
    patch.clear();
    ProgramFileInfo infoTest;
    infoTest._path = "E:\\Alpha2\\Alpha2_QT_Mini\\AlphaRobot2\\AlphaRobot\\Win32\\Debug";
    infoTest._url = "http://sw.bos.baidu.com/sw-search-sp/software/92a30ef6d1820/Baidu_FoxitReader_8.3.3.26761.exe";
    infoTest._name = "1.exe";
    patch.push_back(infoTest);
    infoTest._url = "http://sw.bos.baidu.com/sw-search-sp/software/92a30ef6d1820/Baidu_FoxitReader_8.3.3.26761.exe";
    infoTest._name = "2.exe";
    patch.push_back(infoTest);
    infoTest._url = "http://sw.bos.baidu.com/sw-search-sp/software/92a30ef6d1820/Baidu_FoxitReader_8.3.3.26761.exe";
    infoTest._name = "3.exe";
    patch.push_back(infoTest);
    return true;
}


map<QString, ProgramFileInfo> UpdaterInterface::parseRemoteVersionInfo(const QString &qsXmlPath)
{
    return map<QString, ProgramFileInfo>();
}


map<QString, ProgramFileInfo> UpdaterInterface::localFileList()
{
    const QString qsBaseDir = CConfigs::getAppBasicDataDir();
    QStringList fileInfoList = FileUtils::getFileList(qsBaseDir);
    map<QString, ProgramFileInfo> infos;
    foreach (QString qs , fileInfoList)
    {
        ProgramFileInfo info;
        QString qsName = FileUtils::getFileName(qs);
        info._path = qs;
        info._md5 = FileUtils::getMd5(qs);
        info._url = m_qsBaseUrl + "/" + qsName;
        //infos.insert((qsName, info));
    }
    return infos;
}


QString UpdaterInterface::getRemoteVersion()
{
    HttpRequest request;
    request.set_url("");
    //HttpClient::postRequest();
    return "";
}
