
#ifndef ROBOTCMD_H
#define ROBOTCMD_H

#include "global.h"
#include "type.h"

// jianjie
class Q_DECL_EXPORT CRobotCmd
{
public:
	CRobotCmd(void);
    virtual ~CRobotCmd(void);

	
	byte* GetCmd_ReadHardwareVer(int& nDataLenRet);

	
	byte* GetCmd_ReadSoftwareVer(int& nDataLenRet);

	
    byte* GetCmd_DownloadBinPrepare(const char *sBinSrcPath, const char *sBinDestPath, BOOL bUTF8, int& nDataLenRet);

	
	byte* GetCmd_DownloadBinIng(const byte* curdata, const int curframe, int& nDataLenRet);

	
	byte* GetCmd_DownloadBinEnd(const byte* curdata, const int cursize, int& nDataLenRet);

	
	byte* GetCmd_MotorGetOffset(int nMotorID, int& nDataLenRet);

	
	byte* GetCmd_MotorSetOffset(int nMotorID, int nOffset, int& nDataLenRet);

	
	byte* GetCmd_MotorVersion(int nMotorID, int& nDataLenRet);

	
	byte* GetCmd_MotorGetID(int& nDataLenRet);

	
	byte* GetCmd_MotorSetID(int nOldMotorID, int nNewMotorID, int& nDataLenRet);

	
	byte* GetCmd_MotorUpgradePrepare(int nMotorID, int& nDataLenRet);

	
	byte* GetCmd_MotorUpgradeIng(int nMotorID, int nCurPage, byte* pUserData, int nUserDataLen, int& nDataLenRet);

	
	byte* GetCmd_MotorUpgradeEnd(int nMotorID, int& nDataLenRet);

	
    byte* GetCmd_MotorMove(int nMotorID, int nAngle, int nTime, int& nDataLenRet);

	
	byte* GetCmd_MotorGetState(int nMotorID, int& nDataLenRet);

	
	byte* GetCmd_RobotDebug(MOTOR_DATA* pMotorBuf, int nMotorCount, int nRunTime, int nAllTime, int& nDataLenRet);

	
	byte* GetCmd_SwitchMode(int nMode, int& nDataLenRet);

	
	byte* GetCmd_ConnectRobot(int& nDataLenRet);

	
	byte* GetCmd_DisconnectRobot(int& nDataLenRet);

	
	byte* GetCmd_RobotReset(MOTOR_DATA* pMotorBuf, int nMotorCount, int nRunTime, int nAllTime, int& nDataLenRet);

	
	byte* GetCmd_RobotReadSN(int& nDataLenRet);

	
	byte* GetCmd_RobotWriteSN(char* pszSN, int nSNlen, int& nDataLenRet);

	
	byte* GetCmd_RobotReadUID(int& nDataLenRet);

	
	byte* GetCmd_RobotCheckUTF8(int& nDataLenRet);

    
    byte* GetCmd_MotorGetAngle(int nMotorID, int& nDataLenRet);

};

#endif
