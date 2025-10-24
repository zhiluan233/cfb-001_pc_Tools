
#include "stable.h"
#include "ActionBlockData.h"
#include "cfbdatabase.h"

CActionBlockData::CActionBlockData(void)
{
	m_pBlockData = NULL;
    m_nRunTime = 0;
	//m_fFreezeTime = 0;
    m_nStopTime = 0;
	m_ID = -1;
//	memset(m_strDescription, 0, sizeof(m_strDescription));
	m_nBlockDataLen = 0;
}

CActionBlockData::~CActionBlockData(void)
{
	if (m_pBlockData)
	{
		delete [] m_pBlockData;
		m_pBlockData = NULL;
	}
}


void CActionBlockData::SetDescription(WCHAR* strDescription)
{
    memset(m_strDescription,0, MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
    memcpy(m_strDescription, strDescription ,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
//    memcpy(m_strDescription, strDescription ,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
//    memcpy(m_strDescription, strDescription, MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
}


WCHAR* CActionBlockData::GetDescription()
{
	return m_strDescription;
}


int CActionBlockData::GetID()
{
	return m_ID;
}


void CActionBlockData::SetID(int nID)
{
	m_ID = nID;
}


void CActionBlockData::SetExtBlockData(int nLen, char* pData)
{
	m_nBlockDataLen = nLen;
	if (m_pBlockData)
	{
		delete [] m_pBlockData;
		m_pBlockData = NULL;
	}

	if (nLen)
	{
		m_pBlockData = new char[nLen];
        Q_ASSERT(m_pBlockData);
		memcpy(m_pBlockData, pData, nLen);
	}
}


void CActionBlockData::GetExtBlockData(char** ppData, int& nLen)
{
	nLen = m_nBlockDataLen;
	if (nLen)
	{
		*ppData = new char[nLen];
		memcpy(*ppData, m_pBlockData, nLen);
	}
}


bool CActionBlockData::GetData(char** ppData, int& nLen)
{
    std::vector<char> DataSave;
	//vector<char> DataSaveTest;
	// 数据长度（INT） + DATA
	AddVectorData(&DataSave, (char*)&nLen, sizeof(nLen));
	//nt m_ID;			// 模块ID
	AddVectorData(&DataSave, (char*)&m_ID, sizeof(m_ID));

    AddVectorData(&DataSave, (char*)&m_nRunTime, sizeof(m_nRunTime));//运行时间
	//int m_EndTime;		// 终止时间
    AddVectorData(&DataSave, (char*)&m_nStopTime, sizeof(m_nStopTime));//总时间
	//WCHAR	m_strDescription[MAX_DESCRIPTION_SIZE];	// 描述
	AddVectorData(&DataSave, (char*)m_strDescription, sizeof(m_strDescription));

	// int m_nBlockDataLen;
	AddVectorData(&DataSave, (char*)&m_nBlockDataLen, sizeof(m_nBlockDataLen));
	if (m_nBlockDataLen)
	{
		// char* m_pBlockData;
		AddVectorData(&DataSave, m_pBlockData, m_nBlockDataLen);
		//AddVectorData(&DataSaveTest, m_pBlockData, m_nBlockDataLen);
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


void CActionBlockData::SetData(char* pData, int nLen)
{
	vector<char> DataSaveTest;

	// 数据长度（INT） + DATA
	int nTotalLen = 0;
	CFB_MEMCPY_INC(nTotalLen, pData);
	if (nTotalLen != nLen)
		return;

	//nt m_ID;			// 模块ID
	CFB_MEMCPY_INC(m_ID, pData);
    //int m_nRunTime;	// 运行时间
    CFB_MEMCPY_INC(m_nRunTime, pData); //运行时间
    //int m_nStopTime;		// 停止时间
    CFB_MEMCPY_INC(m_nStopTime, pData);//停止时间

    qDebug()<< "actionFrame runtime: "<<m_nRunTime<<", waitTime: "<<m_nStopTime;

	//WCHAR	m_strDescription[MAX_DESCRIPTION_SIZE];	// 描述
	memcpy(m_strDescription, pData, sizeof(m_strDescription));
	pData += sizeof(m_strDescription);

	// int m_nBlockDataLen;
	CFB_MEMCPY_INC(m_nBlockDataLen, pData);
	if (m_nBlockDataLen)
	{
		// char* m_pBlockData;
		//AddVectorData(&DataSave, m_pBlockData, m_nBlockDataLen);
		if (m_pBlockData)
		{
			delete [] m_pBlockData;
		}

		m_pBlockData = new char[m_nBlockDataLen];
		memcpy(m_pBlockData, pData, m_nBlockDataLen);

		AddVectorData(&DataSaveTest, m_pBlockData, m_nBlockDataLen);

		pData += m_nBlockDataLen;
	}
}


int CActionBlockData::GetMotorAngle(int nMotroID)
{
	//char *pData = NULL;
	int nLen = m_nBlockDataLen;
	//GetExtBlockData()
	if (m_pBlockData == NULL || nLen <= 4)
		return -1;

	MOTOR_DATA* pMotor = (MOTOR_DATA*)(m_pBlockData+sizeof(nLen));
	int nCount = (nLen-sizeof(nLen))/sizeof(MOTOR_DATA);
	for (int i=0; i<nCount; i++)
	{
		//CCFBRobotMotorItemDialog* pItem = FindMotor(pMotor[i].nID);
		if (nMotroID == pMotor[i].nID)
		{
			return pMotor[i].nAngle;
		}
	}
	return -1;
}


void CActionBlockData::SetMotorAngle(char* pData, int nLen, int nID, int nAngle)
{
	int nCount = (nLen-4)/sizeof(MOTOR_DATA);
	MOTOR_DATA* p = (MOTOR_DATA*)(pData+4);
    p[nID].nAngle = (int)nAngle;
}


void CActionBlockData::SetRunTime(int nTime)
{
    m_nRunTime = nTime;
}


int CActionBlockData::GetRunTime()
{
    return m_nRunTime;
}


void CActionBlockData::SetStopTime(int nTime)
{
    m_nStopTime = nTime;
}


int CActionBlockData::GetStopTime()
{
    return m_nStopTime;
}
