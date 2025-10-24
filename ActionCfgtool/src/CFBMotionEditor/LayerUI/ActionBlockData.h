
#ifndef _ACTIONBLOCKDATA_H_
#define _ACTIONBLOCKDATA_H_

#include "stable.h"
#include "type.h"
#include "motioneditor_global.h"

#define MAX_DESCRIPTION_SIZE 30

//该类所有时间对应的都是格数，具体时间要乘以时间粒度才是真实的时间
class MOTIONEDITOR_EXPORT CActionBlockData
{
public:
	CActionBlockData(void);
    virtual ~CActionBlockData(void);

public:
	
	virtual bool GetData(char** ppData, int& nLen);

	
	virtual void SetData(char* pData, int nLen);

	
	int GetID();

	
	void SetID(int nID);

	
	int  GetMotorAngle(int nMotroID);

	
    void SetMotorAngle(char* pData, int nLen, int nID, int nAngle);
	
	
    void SetRunTime(int nTime);

	
    int GetRunTime();

	
    void SetStopTime(int nTime);

	
    int GetStopTime();

	
    void SetDescription(WCHAR* strDescription);

	
    WCHAR *GetDescription();

	
	void SetExtBlockData(int nLen, char* pData);

	
	void GetExtBlockData(char** ppData, int& nLen);

protected:
	int m_ID;			// 模块ID
    int m_nRunTime;   // 运行时间，已经变换
	//float m_fFreezeTime;// 冻结时间，已经变换
    int m_nStopTime;   // 总时间，已经变换
    WCHAR  m_strDescription[MAX_DESCRIPTION_SIZE];	// 描述

	char* m_pBlockData;
	int m_nBlockDataLen;
};
#endif  //_ACTIONBLOCKDATA_H_
