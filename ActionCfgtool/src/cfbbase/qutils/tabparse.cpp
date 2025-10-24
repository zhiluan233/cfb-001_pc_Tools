
#include "tabparse.h"

CTabParser::CTabParser(void)
{
}

CTabParser::~CTabParser(void)
{	

}

Function* CTabParser::GetCacheFunction(const QString& sFuncName)
{
	MapFunctionType::iterator itor = m_mapFunctionCache.find(sFuncName);
	if(itor != m_mapFunctionCache.end())		
		return itor->second;
	return NULL;
}


Function* CTabParser::GetCacheFunction(const Function* pFunc)
{
	MapFunctionType::iterator itor = m_mapFunctionCache.begin();
	while(itor != m_mapFunctionCache.end())		
	{
		if(itor->second == pFunc)
			return itor->second;
		itor++;
	}
	return NULL;
}


BOOL CTabParser::ExistCacheFunction(const QString &sFuncName)
{
	return GetCacheFunction(sFuncName) != NULL;
}


BOOL CTabParser::ExistCacheFunction(const Function* pFunc)
{
	return GetCacheFunction(pFunc) != NULL;
}


void CTabParser::AddCacheFunction(const QString &sFuncName, Function* pFunc)
{
	m_mapFunctionCache[sFuncName] = pFunc;
}


void CTabParser::GetDataFunctionList(Function* pRootFunc, vector<Function*>& vecFuncList)
{
	if(pRootFunc)
	{
        for(size_t i=0; i < pRootFunc->m_vecTokenList.size(); i++)
		{
			Token* pToken = pRootFunc->m_vecTokenList[i];
			DataToken* pDataToken = dynamic_cast<DataToken*>(pToken);
			if(pDataToken)
			{
				vecFuncList.push_back(pRootFunc);
				break;
			}
			else
			{
				FunToken* pFunToken = dynamic_cast<FunToken*>(pToken);
				if(pFunToken)
				{
					GetDataFunctionList(pFunToken->m_pCallFunction, vecFuncList);
				}				
			}
		}		
	}	
}


BOOL CTabParser::ReadTab(const QString &sTabPath, vector<Function*>& vecFuncList)
{
	ReleaseCache();

    QFile fileobj(sTabPath);
    if(fileobj.open(QIODevice::ReadOnly | QIODevice::Text))
	{
        QString sLineStr;
        QString sCurDefineFuncName;//当前定义的函数名
		Function* pMainFunc = NULL;//主函数

        char szBuf[STRING_LEN_1024] = {0};
        while(!fileobj.atEnd())
		{
            memset(szBuf, 0, STRING_LEN_1024);
            fileobj.readLine(szBuf, STRING_LEN_1024);

            sLineStr = QString::fromStdString(szBuf);
            sLineStr = sLineStr.trimmed();

            QString sFindStr = sLineStr;
            sFindStr = sFindStr.toUpper();

            if(sFindStr.indexOf("FUNCTION") != -1)
			{
                QString sFuncName = sLineStr.mid(8, sLineStr.indexOf('(') - 8);
                sFuncName = sFuncName.trimmed();

				sCurDefineFuncName = sFuncName;//记录当前定义的函数名

				if(!ExistCacheFunction(sFuncName))
				{
					//不存在该函数则添加
					Function* pNewFunc = new Function(sFuncName);
					AddCacheFunction(sFuncName, pNewFunc);

                    if(sFuncName == "Main")
						pMainFunc = pNewFunc;
				}
			}
            else if(sFindStr.indexOf("MOVE") != -1)
			{
				Function* pCurFunc = GetCacheFunction(sCurDefineFuncName);
				if(pCurFunc)
				{
					DataToken* pNewDataToken = new DataToken;
					pCurFunc->m_vecTokenList.push_back(pNewDataToken);

                    QString sDataStr = sLineStr.mid(4);
                    sDataStr = sDataStr.trimmed();
                    sDataStr = sDataStr.right(sDataStr.length() - 1);//去掉左括号
                    sDataStr = sDataStr.left(sDataStr.length() - 1);//去掉右括号
                    sDataStr = sDataStr.trimmed();

					int nLastIndex = 0;
                    for(int i=0;i<sDataStr.length();i++)
					{
						if(sDataStr[i] == ',')
						{
                            QString sNum = sDataStr.mid(nLastIndex, i - nLastIndex);
                            sNum = sNum.trimmed();
                            pNewDataToken->m_vecDataFrame.push_back(sNum.toInt());
							nLastIndex = i + 1;
						}
					}

                    QString sNum = sDataStr.mid(nLastIndex);
                    sNum = sNum.trimmed();
                    pNewDataToken->m_vecDataFrame.push_back(sNum.toInt());
				}					
			}
            else if(sFindStr.indexOf("END") != -1)
			{
			}
			else
			{
                if(sLineStr.indexOf('(') != -1 && sLineStr.indexOf(')') != -1)
				{
					//函数调用
					Function* pCurFunc = GetCacheFunction(sCurDefineFuncName);
					if(pCurFunc)
					{
						FunToken* pNewFunToken = new FunToken;

                        QString sCallFuncName = sLineStr.left(sLineStr.indexOf('('));
                        sCallFuncName = sCallFuncName.trimmed();

						Function* pCallFunc = GetCacheFunction(sCallFuncName);
						if(!pCallFunc)
						{
							pCallFunc = new Function(sCallFuncName);	
							AddCacheFunction(sCallFuncName, pCallFunc);//加入缓存
						}

						pNewFunToken->m_pCallFunction = pCallFunc;
						pCurFunc->m_vecTokenList.push_back(pNewFunToken);
					}
				}
			}
		}

        fileobj.close();

		GetDataFunctionList(pMainFunc, vecFuncList);	
		return TRUE;
	}

	return FALSE;
}


BOOL CTabParser::WriteTab(const Function* pMainFunc, const QString &sSavePath)
{
    QFile fileobj(sSavePath);
    if(fileobj.open(QIODevice::WriteOnly | QIODevice::Text))
	{
        vector<QString> vecLineStrList;

		WriteTab(pMainFunc, vecLineStrList);
        for(size_t i = 0; i < vecLineStrList.size(); i++)
		{
            QString sTemp = vecLineStrList[i];
            string szStr = sTemp.toStdString();
            fileobj.write(szStr.c_str(), szStr.length());
			if(i < vecLineStrList.size() - 1)
                fileobj.write("\r\n", 2);
		}

        fileobj.close();
	}
	return TRUE;
}


void CTabParser::WriteTab(const Function* pRootFunc, vector<QString>& vecLineStrList)
{
	if(pRootFunc)
	{
		Function* pCurFunc = const_cast<Function*>(pRootFunc);
        QString sLineStr = "function " + pCurFunc->m_sName + "()";//函数定义开始
		vecLineStrList.push_back(sLineStr);

        sLineStr = "end";//函数定义结束
		vecLineStrList.push_back(sLineStr);

		if(!ExistCacheFunction(pCurFunc))
			AddCacheFunction(pCurFunc->m_sName, pCurFunc);

        for(size_t j = 0; j < pCurFunc->m_vecTokenList.size(); j++)
		{
			Token* pToken = pCurFunc->m_vecTokenList[j];

			DataToken* pDataToken = dynamic_cast<DataToken*>(pToken);
			if(pDataToken)
			{
				//move调用
                sLineStr = "    Move(";
                for(size_t k = 0; k < pDataToken->m_vecDataFrame.size(); k++)
				{
                    QString sNum;
                    sNum.sprintf("%5d", pDataToken->m_vecDataFrame[k]);
					if(k > 0)
                        sNum.insert(0, ", ");//插入逗号分隔符
					sLineStr += sNum;
				}

                sLineStr += ")";//move指令构造结束
                vector<QString>::iterator itorInsert = FindFunctionInsert(vecLineStrList, pCurFunc);
				vecLineStrList.insert(itorInsert, sLineStr);
			}
			else
			{
				FunToken* pFuncToken = dynamic_cast<FunToken*>(pToken);
				if(pFuncToken)
				{
					//子函数调用
                    sLineStr = "    " + pFuncToken->m_pCallFunction->m_sName +  "()";
                    vector<QString>::iterator itorAppend = FindFunctionAppend(vecLineStrList);
					vecLineStrList.insert(itorAppend, sLineStr);

					if(!ExistFunction(vecLineStrList, pFuncToken->m_pCallFunction->m_sName))
					{
						//创建函数定义
						WriteTab(pFuncToken->m_pCallFunction, vecLineStrList);
					}
				}
			}
		}		
	}
}


void CTabParser::ReleaseCache()
{
    Function* pMainFunc = GetCacheFunction("Main");
	ReleaseCache(pMainFunc);
}


void CTabParser::ReleaseCache(Function* pRootFunc)
{
	if(pRootFunc && ExistCacheFunction(pRootFunc))
	{
		vector<Token*>::iterator itor = pRootFunc->m_vecTokenList.begin();
		while(itor != pRootFunc->m_vecTokenList.end())
		{
			FunToken* pFunToken = dynamic_cast<FunToken*>(*itor);
			if(pFunToken)
			{	
				ReleaseCache(pFunToken->m_pCallFunction);
			}

			delete *itor;
			itor = pRootFunc->m_vecTokenList.erase(itor);						
		}	

		RemoveCacheFunction(pRootFunc);
	}
}


void CTabParser::RemoveCacheFunction(const QString &sFuncName)
{
	MapFunctionType::iterator itor = m_mapFunctionCache.find(sFuncName);
	if(itor != m_mapFunctionCache.end())
	{
		delete itor->second;
		m_mapFunctionCache.erase(itor);
	}
}


void CTabParser::RemoveCacheFunction(const Function* pFunc)
{
	MapFunctionType::iterator itor = m_mapFunctionCache.begin();
	while(itor != m_mapFunctionCache.end())
	{
		if(itor->second == pFunc)
		{
			delete itor->second;
            m_mapFunctionCache.erase(itor++);
		}
		else
			itor++;
	}
}


vector<QString>::iterator CTabParser::FindFunctionInsert(vector<QString> &vecLineStrList, const Function* pFunc)
{
    vector<QString>::iterator itorRet = vecLineStrList.begin();
    vector<QString>::iterator itor = itorRet;
	while(itor != vecLineStrList.end())
	{
        QString sLineStr = *itor;
        if(sLineStr.indexOf("function") != -1)
		{
            sLineStr = sLineStr.mid(8, sLineStr.indexOf('(') - 8);
            sLineStr = sLineStr.trimmed();
			if(sLineStr == pFunc->m_sName)
			{
				itorRet = itor + 1;//function行的下一行字符串位置
			}
		}
        else if(sLineStr.indexOf("end") != -1)
		{
			if(itor >= itorRet)
			{
				itorRet = itor;//找到了插入位置
				break;
			}
		}

		itor++;
	}

	return itorRet;
}


vector<QString>::iterator CTabParser::FindFunctionAppend(vector<QString>& vecLineStrList)
{
    vector<QString>::iterator itorRet = vecLineStrList.begin();
    vector<QString>::iterator itor = itorRet;
	while(itor != vecLineStrList.end())
	{
		itorRet = itor;
		itor++;
	}

	return itorRet;
}


BOOL CTabParser::ExistFunction(vector<QString> &vecLineStrList, const QString &sFuncName)
{
    vector<QString>::iterator itor = vecLineStrList.begin();
	while(itor != vecLineStrList.end())
	{
        QString sLineStr = *itor;
        if(sLineStr.indexOf("function") != -1)
		{
            sLineStr = sLineStr.mid(8, sLineStr.indexOf('(') - 8);
            sLineStr = sLineStr.trimmed();
			if(sLineStr == sFuncName)
			{
				return TRUE;
			}
		}

		itor++;
	}

	return FALSE;
}
