#include "stable.h"
#include "ActionBlockGroupManager.h"
#include "util.h"
#include "cfbdatabase.h"

CActionBlockGroupManager::CActionBlockGroupManager(void)
{
    m_nStartTime = 0;
    m_nEndTime = 100;
    m_pBlockData = NULL;
    m_nUnUseBaseID = 0;
    memset(m_strDescription, 0, MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
}


CActionBlockGroupManager::~CActionBlockGroupManager(void)
{
    if (m_pBlockData)
    {
        delete [] m_pBlockData;
        m_pBlockData = NULL;
    }
}


bool CActionBlockGroupManager::GetData(char** ppData, int &nLen)
{
    vector<char> DataSave;
    // DataLen（int） + BlockNUM(INT) + [BLOCKDATALEN + BALOCKDATA]
    AddVectorData(&DataSave, (char*)&nLen, sizeof(nLen));

    //nt m_ID;			// 模块ID
    AddVectorData(&DataSave, (char*)&m_nID, sizeof(m_nID));
    //int m_StartTime;	// 起始时间

    AddVectorData(&DataSave, (char*)&m_nStartTime, sizeof(m_nStartTime));
    //int m_EndTime;		// 终止时间
    AddVectorData(&DataSave, (char*)&m_nEndTime, sizeof(m_nEndTime));
    //WCHAR	m_strDescription[MAX_DESCRIPTION_SIZE];	// 描述
    AddVectorData(&DataSave, (char*)m_strDescription, sizeof(m_strDescription));


    int nBlockCount = m_ListActionBlockData.count();
    AddVectorData(&DataSave, (char*)&nBlockCount, sizeof(nBlockCount));

    for(int i=0; i<nBlockCount; i++)
    {
        CActionBlockData* pBlockData = m_ListActionBlockData.at(i);
        if (pBlockData == NULL)
            continue;

        char* pBlock = NULL;
        int nBlockDataLen = 0;
        pBlockData->GetData(&pBlock, nBlockDataLen);
        // data lens
        AddVectorData(&DataSave, (char*)&nBlockDataLen, sizeof(nBlockDataLen));
        if (pBlock && nBlockDataLen)
        {
            // data
            AddVectorData(&DataSave, pBlock, nBlockDataLen);
            delete [] pBlock;
        }
    }

    nLen = DataSave.size();
    ModifyVertorData(DataSave, 0, (char*)&nLen, sizeof(nLen));
    if (nLen)
    {
        *ppData = new char[nLen];
        Q_ASSERT(*ppData);
        CopyVertorToPointer(DataSave, *ppData);
    }

    return true;
}


bool CActionBlockGroupManager::SetData(char* pData, int nLen)
{
    // 数据长度（INT） + DATA
    int nTotalLen = 0;
    CFB_MEMCPY_INC(nTotalLen, pData);
    if (nTotalLen != nLen)
        return false;
    CFB_MEMCPY_INC(m_nID, pData);
    //int n1 = sizeof(m_nStartTime);
    //int n2 = sizeof(int);

    CFB_MEMCPY_INC(m_nStartTime, pData);
    CFB_MEMCPY_INC(m_nEndTime, pData);
    qDebug()<< "actionGroup runtime: "<<m_nStartTime<<", waittime: "<<m_nEndTime;

    int nSize = sizeof(m_strDescription);  //12/23新加
    memcpy(m_strDescription, pData, nSize);
    pData += nSize;

    //    QString stTmp = WCHARToQString(m_strDescription,nSize);

    // Block num
    int nBlockNum = 0;
    CFB_MEMCPY_INC(nBlockNum, pData);

    for (int i=0; i<nBlockNum; i++)
    {
        char* pBlockData = NULL;
        int nBlockDataLen = 0;
        // block data len
        CFB_MEMCPY_INC(nBlockDataLen, pData);
        if (nBlockDataLen)
        {
            pBlockData = new char[nBlockDataLen];
            memcpy(pBlockData, pData, nBlockDataLen);
            pData += nBlockDataLen;

            CActionBlockData* pActionData =  new CActionBlockData();
            if (pActionData)
            {
                pActionData->SetData(pBlockData, nBlockDataLen);
                AddLayerBlock(pActionData);

            }
            delete [] pBlockData;
        }
    }
    //设置没有使用过的ID
    SetUnUseBaseID();

    return true;
}


//void CActionBlockGroupManager::SetExtBlockData(int nLen, char* pData)
//{
//    m_nBlockDataLen = nLen;
//    if (m_pBlockData)
//    {
//        delete [] m_pBlockData;
//        m_pBlockData = NULL;
//    }

//    if (nLen)
//    {
//        m_pBlockData = new char[nLen];
//        Q_ASSERT(m_pBlockData);
//        memcpy(m_pBlockData, pData, nLen);
//    }
//}


//void CActionBlockGroupManager::GetExtBlockData(char** ppData, int& nLen)
//{
//    nLen = m_nBlockDataLen;
//    if (nLen)
//    {
//        *ppData = new char[nLen];
//        memcpy(*ppData, m_pBlockData, nLen);
//    }
//}


bool CActionBlockGroupManager::AddLayerBlock(CActionBlockData* pBlockData)
{	
    if (pBlockData == NULL)
        return false;
    m_ListActionBlockData.append(pBlockData);

    return true;
}


bool CActionBlockGroupManager::DeleteLayerBlock(int nID)
{

    //Modified Flag
    CActionBlockData* itemFound = NULL;

    for (int i=0; i<m_ListActionBlockData.count(); i++)
    {
        itemFound = m_ListActionBlockData.at(i);
        if (itemFound && itemFound->GetID() == nID)
        {
            m_ListActionBlockData.removeAt(i);
            delete itemFound;
            itemFound = NULL;
            return true;
        }
    }

    return false;
}


void CActionBlockGroupManager::ClearBlockDataList()
{
    CActionBlockData* itemFound = NULL;

    for (int i=0; i<m_ListActionBlockData.count(); i++)
    {
        itemFound = m_ListActionBlockData.at(i);
        if (itemFound )
        {
            delete itemFound;
            itemFound = NULL;
        }
    }
    while(!m_ListActionBlockData.isEmpty())
    {
        m_ListActionBlockData.removeFirst();
    }
}


CActionBlockData* CActionBlockGroupManager::FindBlockData(int nID)
{
    CActionBlockData* itemFound = NULL;

    for (int i=0; i<m_ListActionBlockData.count(); i++)
    {
        itemFound = m_ListActionBlockData.at(i);
        if (itemFound && itemFound->GetID() == nID)
        {
            return itemFound;
        }
    }

    return NULL;
}


int CActionBlockGroupManager::GetActionCount()
{
    return m_ListActionBlockData.count();
}

bool CActionBlockGroupManager::Connect(void)
{
    return true;
}


void CActionBlockGroupManager::ReleaseConnection()
{
    ClearBlockDataList();
}


void CActionBlockGroupManager::SetStartTime(int nStartTime)
{
    m_nStartTime = nStartTime;
}


int CActionBlockGroupManager::GetStartTime()
{
    return m_nStartTime;
}


int CActionBlockGroupManager::GetEndTime()
{
    return m_nEndTime;
}


void CActionBlockGroupManager::SetEndTime(int nEndTime)
{
    m_nEndTime = nEndTime;
}


int CActionBlockGroupManager::GetActionTimeByIndex(int nActionIndex)
{
    if (nActionIndex > m_ListActionBlockData.count())
    {
        return -1;
    }
    int nActionTime = GetStartTime();
    CActionBlockData *pActionData = NULL;
    for (int i = 0 ; i< nActionIndex ; i++)
    {
        pActionData = GetAt(i);
        if (pActionData)
        {
            nActionTime += pActionData->GetRunTime() + pActionData->GetStopTime();
        }
    }
    return nActionTime;
}


void CActionBlockGroupManager::SetDescription(WCHAR* strDescription)
{
    memset(m_strDescription,0, sizeof(m_strDescription));
    memcpy(m_strDescription, strDescription ,sizeof(m_strDescription));
    //    wcsncpy(m_strDescription, strDescription, MAX_DESCRIPTION_SIZE-1);
}


WCHAR* CActionBlockGroupManager::GetDescription()
{
    return m_strDescription;
}


CActionBlockData* CActionBlockGroupManager::GetAt(int nIndex)
{
    if(nIndex >= m_ListActionBlockData.count())
    {
        return NULL;
    }
    return m_ListActionBlockData.at(nIndex);
}


int CActionBlockGroupManager::GetIndexByID(int nID)
{
    int nIndex = 0;
    CActionBlockData* itemFound = NULL;

    for (; nIndex < m_ListActionBlockData.count(); ++nIndex)
    {
        itemFound = m_ListActionBlockData.at(nIndex);
        if (itemFound && itemFound->GetID() == nID)
        {
            break;
        }
    }

    return nIndex;
}


int CActionBlockGroupManager::GetID()
{
    return m_nID;
}


void CActionBlockGroupManager::SetID(int nID)
{
    m_nID = nID;
}


void CActionBlockGroupManager::GetAllBlockData(vector<CActionBlockData*>& blockDataList)
{
    CActionBlockData* itemFound = NULL;
    for (int i=0; i < m_ListActionBlockData.count(); i++)
    {
        itemFound = m_ListActionBlockData.at(i);
        blockDataList.push_back(itemFound);
    }
}


int CActionBlockGroupManager::GetUnUserID()
{

    return ++m_nUnUseBaseID;
}


void CActionBlockGroupManager::RecalculateTime()
{
    int nEndTime = GetStartTime();
    CActionBlockData* itemFound = NULL;
    for (int i=0; i<m_ListActionBlockData.count(); i++)
    {
        itemFound = m_ListActionBlockData.at(i);
        if (itemFound)
        {
            nEndTime += itemFound->GetRunTime() + itemFound->GetStopTime();
        }
    }

    SetEndTime(nEndTime);
}


void CActionBlockGroupManager::InsertBeforeId(int nID, CActionBlockData* pActionBlock)
{
    for (int i=0; i<GetActionCount(); i++)
    {
        CActionBlockData* pBlock = GetAt(i);
        if (pBlock->GetID() == nID)
        {
            if (pActionBlock->GetID() == -1)
            {
                pActionBlock->SetID(GetUnUserID());
            }

            int  pos = m_ListActionBlockData.indexOf(pBlock);
            if (pos < 0)
            {
                m_ListActionBlockData.prepend(pActionBlock);
            }
            else
            {
                m_ListActionBlockData.insert(pos, pActionBlock);
            }
            return;
        }
    }
}


void CActionBlockGroupManager::InsertAfterId(int nID, CActionBlockData* pActionBlock)
{
    for (int i=0; i<GetActionCount(); i++)
    {
        CActionBlockData* pBlock = GetAt(i);
        if (pBlock->GetID() == nID)
        {
            if (pActionBlock->GetID() == -1)
            {
                pActionBlock->SetID(GetUnUserID());
            }

            int  pos = m_ListActionBlockData.indexOf(pBlock);
            if (pos < 0)
            {
                m_ListActionBlockData.prepend(pActionBlock);
            }
            else
            {
                m_ListActionBlockData.insert(pos, pActionBlock);
            }
            return;
        }
    }
}

//
//int CActionBlockGroupManager::GetActionRunTimeSum()
//{
//    int nRunTime = 0;
//    for (int i = 0;i<GetActionCount();i++)
//    {
//        CActionBlockData *pData = GetAt(i);
//        if (pData)
//        {
//            nRunTime += pData->GetRunTime();
//        }
//    }
//    return nRunTime;
//}

//
//int CActionBlockGroupManager::GetActionAllTimeSum()
//{
//    int nRunTime = 0;
//    for (int i = 0;i<GetActionCount();i++)
//    {
//        CActionBlockData *pData = GetAt(i);
//        if (pData)
//        {
//            nRunTime += pData->GetAllTime();
//        }
//    }
//    return nRunTime;
//}


void CActionBlockGroupManager::SetUnUseBaseID()
{
    int nBaseID = 0;
    CActionBlockData* itemFound = NULL;
    for (int i=0; i<m_ListActionBlockData.count(); i++)
    {
        itemFound = m_ListActionBlockData.at(i);
        if(itemFound->GetID() > nBaseID)
        {
            nBaseID = itemFound->GetID();
        }
    }
    m_nUnUseBaseID = nBaseID;
}
