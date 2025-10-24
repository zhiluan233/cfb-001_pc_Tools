
#ifndef _TABPARSER_H_
#define _TABPARSER_H_

#include <map>
#include <vector>
#include <QString>
#include <QFile>

using namespace std;

//动作基类
class Token
{
public:	
	virtual ~Token(){}	
};

//函数
class Function
{
public:
    Function(const QString& sName)
	{
		m_sName = sName;
	}	

    QString        m_sName;//函数名称
	vector<Token*> m_vecTokenList;//动作列表
};

//move动作
class DataToken : public Token
{
public:
	~DataToken()
	{
		vector<int>().swap(m_vecDataFrame);
	}
	vector<int> m_vecDataFrame;//数据帧
};

//函数动作
class FunToken : public Token
{
public:
	FunToken()
	{
		m_pCallFunction = NULL;
	}	

	Function* m_pCallFunction;//调用的函数
};

//tab文件解析类
class CTabParser
{
public:
	CTabParser(void);
	~CTabParser(void);

	
    Function* GetCacheFunction(const QString& sFuncName);

	
	Function* GetCacheFunction(const Function* pFunc);

	
    BOOL ExistCacheFunction(const QString& sFuncName);

	
	BOOL ExistCacheFunction(const Function* pFunc);

	
    void AddCacheFunction(const QString& sFuncName, Function* pFunc);

	
	void GetDataFunctionList(Function* pRootFunc, vector<Function*>& vecFuncList);

	
    BOOL ReadTab(const QString& sTabPath, vector<Function*>& vecFuncList);

	
    BOOL WriteTab(const Function* pMainFunc, const QString& sSavePath);

	
	void ReleaseCache();

	
    void RemoveCacheFunction(const QString& sFuncName);

	
	void RemoveCacheFunction(const Function* pFunc);

private:
	
	void ReleaseCache(Function* pRootFunc);

	
    void WriteTab(const Function* pRootFunc, vector<QString>& vecLineStrList);

	
    vector<QString>::iterator FindFunctionInsert(vector<QString>& vecLineStrList, const Function* pFunc);

	
    vector<QString>::iterator FindFunctionAppend(vector<QString>& vecLineStrList);

	
    BOOL ExistFunction(vector<QString>& vecLineStrList, const QString& sFuncName);

private:
    typedef map<QString, Function*> MapFunctionType;//函数名到函数的映射
	MapFunctionType m_mapFunctionCache; 
};
#endif  //_TABPARSER_H_
