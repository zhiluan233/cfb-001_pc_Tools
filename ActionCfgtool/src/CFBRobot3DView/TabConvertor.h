
#ifndef _TABCONVERTOR_H_
#define _TABCONVERTOR_H_

#include "TabParser.h"
#include "util.h"
#include "robotdef.h"

//#ifndef MOTOR_NUM
//#define MOTOR_NUM 16  //舵机数量
//#endif

#ifndef MOTOR_TAB_NUM
#define MOTOR_TAB_NUM 20  //tab文件中的舵机数量
#endif

#ifndef MOTOR_TAB_RUNTIME_COL
#define MOTOR_TAB_RUNTIME_COL MOTOR_TAB_NUM  //tab文件中的运行时间列索引
#endif

#ifndef MOTOR_TAB_ALLTIME_COL
#define MOTOR_TAB_ALLTIME_COL MOTOR_TAB_RUNTIME_COL + 1  //tab文件中的总时间列索引
#endif

#ifndef ACTION_LAYER_TIME
#define ACTION_LAYER_TIME		0
#endif
#ifndef ACTION_LAYER_MOTION
#define ACTION_LAYER_MOTION		1
#endif
#ifndef ACTION_LAYER_EYE
#define ACTION_LAYER_EYE		2
#endif
#ifndef ACTION_LAYER_EAR
#define ACTION_LAYER_EAR		3
#endif
#ifndef ACTION_LAYER_MUSIC
#define ACTION_LAYER_MUSIC		4
#endif

//动作数据
typedef struct _MYACTIONDATA
{
    ~_MYACTIONDATA()
    {
        CUtil::ReleaseVector(vecMotorList);
    }

    //获取舵机数
    int getMotorCount()
    {
        return vecMotorList.size();
    }

	vector<MOTOR_DATA*> vecMotorList;//舵机数据列表
	float fRunTime;//运行时间，单位ms
    float fAllTime;//总时间，单位ms
}MYACTIONDATA;

//分组数据
typedef struct _MYGROUPDATA
{	
    _MYGROUPDATA(const QString& sName)
	{
		sGroupName = sName;
	}
	~_MYGROUPDATA()
	{
		CUtil::ReleaseVector(vecActionList);
	}

    //获取舵机数
    int getMotorCount()
    {
        if(vecActionList.size() > 0)
            return vecActionList[0]->getMotorCount();
        return 0;
    }

    QString               sGroupName;   //分组名称
	vector<MYACTIONDATA*> vecActionList;//动作列表
}MYGROUPDATA;

//tab文件数据
typedef struct _MYTABDATA
{
	~_MYTABDATA()
	{
		CUtil::ReleaseVector(vecGroupList);
	}

    //获取舵机数
    int getMotorCount()
    {
        if(vecGroupList.size() > 0)
            return vecGroupList[0]->getMotorCount();
        return 0;
    }

	vector<MYGROUPDATA*> vecGroupList;//分组列表
}MYTABDATA;

//tab转换类
class CTabConvertor
{
public:
	CTabConvertor(void);
	~CTabConvertor(void);

	
    BOOL GetTabData(const QString &sTabPath, MYTABDATA*& pMyTabData);

	
    BOOL CreateAesData(MYTABDATA* pMyTabData, char*& pData, int& nLen);

	
	BOOL CreateHtsData(const MYTABDATA* pMyTabData, char*& pData, int& nLen);

	
    BOOL CreateAesData(const QString &sTabPath, char*& pData, int& nLen);

	
    BOOL CreateHtsData(const QString &sTabPath, char*& pData, int& nLen);

	
	byte* GetHtsFrameData(int nFrameType, int nTotalFrame, int nCurFrame, int* pMoveData, int nMotorDataLen, int* pRemain);

	
    BOOL CreateHtsData2(const QString &sAesPath, char*& pOutData, int& nOutLen);

    
    BOOL CreateHtsData(byte* pInAesFileData, const ULONGLONG lInLen, char*& pOutData, int& nOutLen);

private:
	CTabParser m_tabparser;//tab解析器	
};

#endif  //_TABCONVERTOR_H_
