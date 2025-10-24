#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QFile>
#include <QNetworkReply>
#include <QProgressBar>
#include "updateparam.h"
#include <QTime>


class UPDATELIBSHARED_EXPORT DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = 0);
    virtual ~DownloadManager ();

signals:
    void            finished ();

public:
    void            startDownLoad ();

public slots:
    void            slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void            slotDownloadFinished();
    void            slotDownloadReadyRead();
    void            slotNetWorkError (QNetworkAccessManager::NetworkAccessibility error);

protected:
    QByteArray      clacMd5 (QString strFilePath);           
    QString         urlToLocalPath (QUrl url);               
    void            installPackge (QString strFilePath);
    void            copyDir (QString strSrcDir, QString strDestDir, bool isOverride = false);
    void            delDir (QString strPath);

public:
    void            setServerParam (UpdateParam* param) { memcpy(m_param, param, sizeof(UpdateParam)); }
    UpdateParam*    getServerParam () const { return m_param; }

protected:
    QNetworkAccessManager   m_pManager;
    QString                 m_strFolder;
    QFile                   m_file;                      // 下载文件数据
    QNetworkReply*          m_pReply;
    UpdateParam*            m_param;                    // 服务器返回数据
    QTime                   m_downloadTime;

protected:
    QProgressBar*           m_progress;
};

#endif // DOWNLOADMANAGER_H
