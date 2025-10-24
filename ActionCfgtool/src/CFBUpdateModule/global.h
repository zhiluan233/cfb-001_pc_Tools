
#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QStringList>
#include "robotdef.h"

#define CHECKRESULT_NOUPDATE   0 //无需更新
#define CHECKRESULT_NEEDUPDATE 1 //需要更新
#define CHECKRESULT_ERROR      2 //更新出错

#define STR_LEN 64
#define URL_LEN 1024
#define DESCRIPTION_LEN 1024 * 3


const QString UPDATE_MODULE_DEFAULT_QSS = ":/res/qss/CFBUpdateModuleDefault.qss";


typedef struct _FILEINFO
{
//	char name[STR_LEN];   //名称，如test.exe
//	char version[STR_LEN];//版本，如1.2.0.0
//	int  size;            //大小，单位字节
//	char date[STR_LEN];   //日期
//	char url[URL_LEN];    //下载地址，如http://www.abc.com/test.exe
//    char description[DESCRIPTION_LEN]; // 版本描述信息

    // Modify by huangwuxian 2016.7.27
    QString m_sName;  // 名称，如test.exe
    QString m_sVersion; //版本，如1.2.0.0
    int m_nSize;  //大小，单位字节
    QString m_sUpdateDateTime;   //日期
    QString m_sUrl;    //下载地址，如http://www.abc.com/test.exe
    QStringList m_slstDescriptions;  // 版本描述信息

	_FILEINFO()
	{
        m_sName = "";
        m_sVersion = "";
        m_nSize = 0;
        m_sUpdateDateTime = "";
        m_sUrl = "";
        m_slstDescriptions.clear();

//		memset(this, 0, sizeof(_FILEINFO));
	}
	_FILEINFO& operator = (const _FILEINFO& fi)
	{
        this->m_sName = fi.m_sName;
        this->m_sVersion = fi.m_sVersion;
        this->m_nSize = fi.m_nSize;
        this->m_sUpdateDateTime = fi.m_sUpdateDateTime;
        for (int num = 0; num < fi.m_slstDescriptions.size(); num++)
        {
            this->m_slstDescriptions.append(fi.m_slstDescriptions[num]);
        }
//		memcpy(this, &fi, sizeof(_FILEINFO));
		return *this;
	}

    void reset()
    {
        m_sName = "";
        m_sVersion = "";
        m_nSize = 0;
        m_sUpdateDateTime = "";
        m_sUrl = "";
        m_slstDescriptions.clear();
    }
}FILEINFO;


int CompareVersion(const QString& sLocalVer, const QString& sUpdateVer, emCheckUpdateType chktype);


int CheckUpdate(const QString& sUpdateURL, FILEINFO& fileinfo, const QString& sLocalVer, emCheckUpdateType chktype);


//BOOL DownloadFile(
//    const QString& sFileURL,
//    const QString& sSavePath,
//	FILEDOWNPROCESSNOTIFY procnotify = NULL, 
//    LPVOID hUser = NULL);


void CancelDownload();


QString GetReqBasicParam();


bool parserUpdateVersionInfo(const char *pJsonInfo, int nSize, FILEINFO& fileinfo);

#endif
