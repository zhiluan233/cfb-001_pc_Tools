#ifndef _ACTIONBLOCKGROUPMANAGER_H_
#define _ACTIONBLOCKGROUPMANAGER_H_

#include "motioneditor_global.h"
#include <vector>
#include "ActionBlockData.h"

using std::vector;

//这里所有的时间都是时间格，具体的时间要乘以时间粒度
class MOTIONEDITOR_EXPORT CActionBlockGroupManager
{
public:
    CActionBlockGroupManager(void);
    virtual ~CActionBlockGroupManager(void);

protected:
    int m_nID;			// 模块ID
    int m_nStartTime;	// 起始时间
    int m_nEndTime;		// 终止时间
    WCHAR  m_strDescription[MAX_DESCRIPTION_SIZE];	// 描述

    char* m_pBlockData;
//    int m_nBlockDataLen;
    int m_nUnUseBaseID;     //动作帧未使用ID基数；

public:
    virtual bool Connect(void);

    
    virtual void ReleaseConnection();

    
    virtual bool GetData(char** ppData, int &nLen);

    
    virtual bool SetData(char* pData, int nLen);

    
    virtual bool DeleteLayerBlock(int nID);

    
    virtual bool AddLayerBlock(CActionBlockData* pBlockData);

    
    virtual void ClearBlockDataList();

//    
//    bool IsExistBlockData(int nID);

    
    CActionBlockData* FindBlockData(int nID);

    
    int GetActionCount();

    
    CActionBlockData* GetAt(int nIndex);

    
    int GetIndexByID(int nID);

    
    int GetUnUserID();

    
    int GetID();

    
    void SetID(int nID);

    
    void SetStartTime(int nStartTime);

    
    int GetStartTime();

    
    int GetActionTimeByIndex(int nActionIndex);

    
    void SetEndTime(int nEndTime);

    
    int GetEndTime();

//    
//    void SetExtBlockData(int nLen, char* pData);

//    
//    void GetExtBlockData(char** ppData, int& nLen);

    
    void SetDescription(WCHAR* strDescription);

    
    WCHAR *GetDescription();

    
    void GetAllBlockData(vector<CActionBlockData*>& blockDataList);

    
    void RecalculateTime();

    
    void InsertBeforeId(int nID, CActionBlockData* pActionBlock);

    
    void InsertAfterId(int nID, CActionBlockData* pActionBlock);

//    
//    int GetActionRunTimeSum();

//    
//    int GetActionAllTimeSum();

    
    void SetUnUseBaseID();

private:
    // 保存所有创建的层的容器
    QList<CActionBlockData*> m_ListActionBlockData;
};
#endif  //_ACTIONBLOCKGROUPMANAGER_H_
