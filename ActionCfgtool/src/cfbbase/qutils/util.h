
#ifndef UTIL_H
#define UTIL_H

#include "qutils_global.h"
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <string>
using namespace std;
#ifdef USEVERSION
#pragma comment(lib,"version.lib")
#endif
#ifdef _WIN32
#include <windows.h>
#include <TlHelp32.h>
#include <shlwapi.h>
#endif

#include <QFile>

#define SAFE_FREE(p) {if(p){ free(p); p = NULL;}}
#define SAFE_DELETE(p) {if(p){ delete p; p = NULL;}}
#define SAFE_DELETE_ARRAY(p) {if(p){ delete [] p; p = NULL;}}
#define SAFE_FILE_CLOSE(p) {if(p){ fclose(p); p = NULL;}}
#define SAFE_CLOSE(p) {if(p){ p->Close(); p = NULL;}}
#define NULLSTR ""


#define STRUCT_CONSTRUCTOR_DEFAULT(T)         \
T()                                           \
{                                             \
	memset(this, 0, sizeof(T));               \
}

#define STRUCT_CONSTRUCTOR_COPY(T)            \
T(const T& obj)                               \
{                                             \
	if (this != &obj)                         \
	{                                         \
		memcpy(this, &obj, sizeof(T));        \
	}                                         \
}


#define STRUCT_OPERATOR_ASSIGN(T)             \
T& operator = (const T& obj)                  \
{                                             \
	if (this != &obj)                         \
	{                                         \
		memcpy(this, &obj, sizeof(T));        \
	}                                         \
	return *this;                             \
}

#define STRUCT_CONSTRUCTOR(T)                 \
T()                                           \
{                                             \
	memset(this, 0, sizeof(T));               \
}                                             \
T(const T& obj)                               \
{                                             \
	if (this != &obj)                         \
	{                                         \
		memcpy(this, &obj, sizeof(T));        \
	}                                         \
}                                             \
T& operator = (const T& obj)                  \
{                                             \
	if (this != &obj)                         \
	{                                         \
		memcpy(this, &obj, sizeof(T));        \
	}                                         \
	return *this;                             \
}


#define CFB_MEMCPY_INC(t, s) memcpy(&t, s, sizeof(t));s+=sizeof(t);

#define TRUE  1
#define FALSE 0
#define NULL  0

#define IN
#define OUT

#ifdef Q_OS_WIN
#define SELECTANY __declspec(selectany)
#else
#define SELECTANY
#endif

#define SAFE_DELETE(p)       if(p){ delete p; p = NULL;}
#define SAFE_DELETE_ARRAY(p) if(p){ delete [] p; p = NULL;}
#define SAFE_FREE(p)         if(p){ free(p); p = NULL;}
#define SAFE_CLOSEHANDLE(p)  if(p){ CloseHandle(p); p = NULL;}

//拷贝内存并移动源数据指针
#define MEMCPY_OFFSETSRC(dest,src,size) {memcpy(dest, src, size); src += size;}

//拷贝内存并移动目标数据指针
#define MEMCPY_OFFSETDEST(dest,src,size) {memcpy(dest, src, size); dest += size;}

//拷贝内存并移动源数据指针和目标数据指针
#define MEMCPY_OFFSETBOTH(dest,src,size) {memcpy(dest, src, size); src += size; dest += size;}

//将符号转为字符串
#define GETSTR(s) #s

#define STRING_LEN_32    32
#define STRING_LEN_64    64
#define STRING_LEN_128   128
#define STRING_LEN_256   256
#define STRING_LEN_512   512
#define STRING_LEN_1024  1024

#ifdef Q_OS_WIN
#define STDCALL __stdcall
#else
#define STDCALL
#define CALLBACK
#endif

#define STRUCT_INIT(T)\
T()\
{\
    memset(this, 0, sizeof(T));\
}\
T(const T& obj)\
{\
    if(this != &obj)\
        memcpy(this, &obj, sizeof(T));\
}\
T& operator = (const T& obj)\
{\
    if(this != &obj)\
        memcpy(this, &obj, sizeof(T));\
    return *this;\
}

#define STRUCT_CONSTRUCT_DEFAULT(T)\
T()\
{\
    memset(this, 0, sizeof(T));\
}

#define STRUCT_CONSTRUCT_COPY(T)\
T(const T& obj)\
{\
    if(this != &obj)\
        memcpy(this, &obj, sizeof(T));\
}

#define STRUCT_OPERATOR_ASSIGN(T)\
T& operator = (const T& obj)\
{\
    if(this != &obj)\
        memcpy(this, &obj, sizeof(T));\
    return *this;\
}

#define ZXPROPERTY(NAME, TYPE, VARIATE)\
    public:                   \
        TYPE Get##NAME()      \
        {					  \
            return VARIATE;   \
        }					  \
        void Set##NAME(TYPE var)\
        {					  \
            VARIATE = var;    \
        }

#define ZXPROPERTY_GET(NAME, TYPE, VARIATE)\
    public:                   \
        TYPE Get##NAME()      \
        {					  \
            return VARIATE;   \
        }

#define ZXPROPERTY_SET(NAME, TYPE, VARIATE)\
    public:                     \
        void Set##NAME(TYPE var)\
        {					    \
            VARIATE = var;      \
        }


#define ZXPROPERTY_REF(NAME, TYPE, VARIATE)\
    public:                   \
        TYPE& Get##NAME()     \
        {					  \
            return VARIATE;   \
        }					  \
        void Set##NAME(const TYPE& var)\
        {					  \
            VARIATE = var;    \
        }

#define ZXPROPERTY_REF_GET(NAME, TYPE, VARIATE)\
    public:                   \
        TYPE& Get##NAME()     \
        {					  \
            return VARIATE;   \
        }

#define ZXPROPERTY_REF_SET(NAME, TYPE, VARIATE)\
    public:                   \
        void Set##NAME(const TYPE& var)\
        {					  \
            VARIATE = var;    \
        }

typedef struct MYDATA
{
        unsigned char* databuf;
	int datalen;
        MYDATA(unsigned char* pdata, int len)
	{
		databuf = pdata;
		datalen = len;
	}
	~MYDATA()
	{
		Release();
	}

	void Release()
	{
		SAFE_DELETE_ARRAY(databuf);
		datalen = 0;
	}

	STRUCT_CONSTRUCTOR(MYDATA)
}MYDATA;

typedef struct FILEVERINFO
{
    QString sFileDesc;      //文件描述
    QString sInternalName;  //内部名称
    QString sLegalCopyright;//合法版权
    QString sOriginName;    //原始文件名
    QString sProductName;   //产品名称，一般是重大升级或改进版本
    QString sProductVer;    //产品版本
    QString sCompanyName;   //公司名称
    QString sFileVer;       //文件版本，每次发布的版本
}FILEVERINFO;

class QUTILS_EXPORT CUtil
{
public:
	CUtil(void);
	virtual ~CUtil(void);

	
    template <typename T>
	static void ReleaseVector(vector<T>& vec);

	
    template <class K, class V>
	static void ReleaseMap(map<K, V>& mapobj);	
	
	
    static vector<QString> StrSplit(const QString& source, char ch);

	
    static vector<QString> StrSplit(const QString& source, char frontch, char rearch);

	
    static vector<QString> StrSplit(const QString& source, const QString& splitstr);

	
    static vector<QString> StrSplit(const QString& source, const QString& frontstr, const QString& rearstr);

	
    template <class T>
	static void ReleaseSet(set<T>& setobj);

	
    template <class T>
	static void ReleaseStack(stack<T>& stackobj);	

	
    template <class T>
    static bool VectorFind(const vector<T>& vecobj, const T& var);

    
    static qint64 GetFileSize(const QString& sFilePath);

    
    static QString GetFileExt(const QString& sFilePath);

    
    static QString GetShortFileName(const QString& sFilePath);

    
    static QString GetQString(wchar_t* pStr, int nLen);

    
    static int GetWString(const QString& str, wchar_t* pWStr);

    
    static QString ReplaceFileExt(const QString& sSrcPath, const QString& sNewExt);

#ifdef USEVERSION
    
    static bool GetFileVersion(const QString& sAppPath, FILEVERINFO& fileinfo);
#endif
    
    static bool GetAppID(const QString& sAppPath, QString& sAppID);

#ifdef _WIN32
    
    static BOOL KillExe(const QString &sProcessName);

    
    static BOOL FindProcessName(const QString &sProcessName, PROCESSENTRY32& pe);

    
    static BOOL FindProcessPath(const QString &sProcessPath, PROCESSENTRY32& pe);
#endif

    
    static QStringList listFilter(const QStringList& inStringList, const QStringList& filterList);

    
    static QString GetFormatSizeString(qint64 nSize);
};


template <typename T>
void CUtil::ReleaseVector(vector<T>& vec)
{
    typename vector<T>::iterator iter;
	for(iter=vec.begin();iter!=vec.end();)
	{
		delete *iter;
		iter = vec.erase(iter);
	}
	vector<T>().swap(vec);
}


template <class K, class V>
void CUtil::ReleaseMap(map<K, V>& mapobj)
{
    class map<K, V>::iterator iter;
	for(iter=mapobj.begin();iter!=mapobj.end();)
	{
		delete iter->second;
		iter = mapobj.erase(iter);
	}
	map<K, V>().swap(mapobj);
}


template <class T>
void CUtil::ReleaseSet(set<T>& setobj)
{
    class set<T>::iterator iter;
	for(iter=setobj.begin();iter!=setobj.end();)
	{
		delete *iter;
		iter = setobj.erase(iter);
	}
	set<T>().swap(setobj);
}


template <class T>
void CUtil::ReleaseStack(stack<T>& stackobj)
{
	while(stackobj.size() > 0)
	{
		delete stackobj.top();
		stackobj.pop();
	}
	stack<T>().swap(stackobj);
}


template <class T>
bool CUtil::VectorFind(const vector<T>& vecobj, const T& var)
{
    class vector<T>::iterator itor = vecobj.begin();
    for(; itor != vecobj.end(); itor++)
	{
		if((*itor) == var)
			return true;
	}

	return false;
}

#endif
