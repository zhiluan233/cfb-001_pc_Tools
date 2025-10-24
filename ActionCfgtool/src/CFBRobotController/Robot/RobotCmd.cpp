
#include "stdafx.h"
#include "RobotCmd.h"
#include "formatfunc.h"
#include "fileutils.h"

CRobotCmd::CRobotCmd(void)
{
}


CRobotCmd::~CRobotCmd(void)
{
}


byte* CRobotCmd::GetCmd_ReadHardwareVer(int& nDataLenRet)
{
	static byte databuf[] = {0xF1, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xED};
	nDataLenRet = sizeof(databuf);
	return databuf;
}


byte* CRobotCmd::GetCmd_ReadSoftwareVer(int& nDataLenRet)
{	
	static byte databuf[] = {0xF5, 0x5F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xED};
	nDataLenRet = sizeof(databuf);
	return databuf;
}


byte* CRobotCmd::GetCmd_DownloadBinPrepare(const char* sBinSrcPath, const char* sBinDestPath, BOOL bUTF8, int& nDataLenRet)
{	
    //QString sSrcPath = QString::fromStdWString(utils::ansi2unicode(sBinSrcPath));
    std::wstring wsSrcPath = utils::ansi2unicode(sBinSrcPath);
    if (PathFileExists(wsSrcPath.c_str()))
	{        
        const char* sztempbinname = sBinDestPath;
		int filenamelen = strlen(sztempbinname) + 1;
#ifndef QT_NO_DEBUG
        string sTestStr = sztempbinname;
#endif
        string sUTF8;
        if(bUTF8) {
            //QString sTempUnicode = AnsiToUnicode(sBinDestPath);
            //char* pszUTF8 = NULL;
            //int nLen = UnicodeToUtf8(sTempUnicode, pszUTF8);
            //sUTF8 = pszUTF8;
            //SAFE_DELETE_ARRAY(pszUTF8);
            sUTF8 = utils::ansi2utf8(sBinDestPath);
            if(sUTF8.length() <= 0)
			{
				nDataLenRet = 0;
				return NULL;
			}
            sztempbinname = sUTF8.c_str();
            filenamelen = sUTF8.length() + 1;
		}

		const int framesize = 58;//数据帧长度		
		int cmdlen = 1 + 1 + filenamelen + 2;
        //int filesize = CUtil::GetFileSize(sSrcPath);
        int filesize = FileUtils::getFileSize(QString::fromStdWString(wsSrcPath));
		int framecount = filesize%framesize ? filesize/framesize+1 : filesize/framesize;//总帧数

		nDataLenRet = 2 + 1 + 1 + filenamelen + 2 + 1 + 1;
		byte* databuf = new byte[nDataLenRet];
		memset(databuf, 0, nDataLenRet);

		byte* databuftemp = databuf;
		byte head[2] = {0xF8, 0x8F};

		memcpy(databuftemp, head, 2);
		databuftemp += 2;

		*databuftemp = cmdlen;
		databuftemp += 1;

		*databuftemp = filenamelen;
		databuftemp += 1;

		memcpy(databuftemp, sztempbinname, filenamelen);
		databuftemp += filenamelen;

		memcpy(databuftemp, &framecount, 2);
		databuftemp += 2;

		byte chksum = (byte)cmdlen + (byte)filenamelen;
		for(int i=0;i<filenamelen;i++)
		{
			chksum += sztempbinname[i];
		}

		byte tempcount[2] = {0};
		memcpy(tempcount, &framecount, 2);
		chksum += tempcount[0] + tempcount[1];		

		*databuftemp = chksum;
		databuftemp += 1;

		*databuftemp = 0xED;

		return databuf;
	}
	return NULL;
}


byte* CRobotCmd::GetCmd_DownloadBinIng(const byte* curdata, const int curframe, int& nDataLenRet)
{
	const int framesize = 58;//数据帧长度
	const int sendsize = 64; //发送长度
	nDataLenRet = sendsize;

	byte* sendbuf = new byte[sendsize];
	memset(sendbuf, 0, sendsize);

	byte* pbuf = sendbuf;

	byte head[2] = {0xF7, 0x7F};
	memcpy(pbuf, head, 2);
	pbuf += 2;

	memcpy(pbuf, &curframe, 2);
	pbuf += 2;

	memcpy(pbuf, curdata, framesize);
	pbuf += framesize;

	byte tempchk[2] = {0};
	memcpy(tempchk, &curframe, 2);

	byte chksum = tempchk[0] + tempchk[1];
	for(int i=0;i<framesize;i++)
	{
		chksum += curdata[i];
	}

	*pbuf = chksum;
	pbuf += 1;

	*pbuf = 0xED;

	return sendbuf;
}


byte* CRobotCmd::GetCmd_DownloadBinEnd(const byte* curdata, const int cursize, int& nDataLenRet)
{
	const int sendsize = 2 + 1 + cursize + 1 + 1;//发送长度
	nDataLenRet = sendsize;

	byte* sendbuf = new byte[sendsize];
    memset(sendbuf, 0, sendsize);

	byte* pbuf = sendbuf;

	byte head[2] = {0xF6, 0x6F};
	memcpy(pbuf, head, 2);
	pbuf += 2;

	byte cmdlen = 1 + cursize;//命令长度
	*pbuf = cmdlen;
	pbuf += 1;

	memcpy(pbuf, curdata, cursize);
	pbuf += cursize;

	byte chksum = cmdlen;
	for(int i=0;i<cursize;i++)
	{
		chksum += curdata[i];
	}

	*pbuf = chksum;
	pbuf += 1;

	*pbuf = 0xED;

	return sendbuf;
}


byte* CRobotCmd::GetCmd_MotorGetOffset(int nMotorID, int& nDataLenRet)
{	
	static byte databuf[] = {0xFA, 0xAF, 0x0F, 0xD4, 0x00, 0x00, 0x00, 0x00, 0x00, 0xED};
	nDataLenRet = sizeof(databuf);

	databuf[2] = nMotorID;

	databuf[8] = GetChkSum(databuf + 2, 6);
	return databuf;
}


byte* CRobotCmd::GetCmd_MotorSetOffset(int nMotorID, int nOffset, int& nDataLenRet)
{
	static byte databuf[] = {0xFA, 0xAF, 0x0F, 0xD2, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0xED};
	nDataLenRet = sizeof(databuf);

	databuf[2] = nMotorID;

//	CString str;
//	str.Format(_T("%02X"), nOffset);
//	str = str.Right(2);

//	int iRes;
//	StrToIntEx((L"0x"+str), STIF_SUPPORT_HEX, &iRes);

	if (nOffset >= 0)
	{
		databuf[7] = nOffset;
		databuf[6] = 0x00;
	}
	else
	{
		databuf[6] = 0xFF;
        databuf[7] = nOffset;
	}

	databuf[8] = GetChkSum(databuf + 2, 6);
	return databuf;
}


byte* CRobotCmd::GetCmd_MotorVersion(int nMotorID, int& nDataLenRet)
{
	static byte databuf[] = {0xFC, 0xCF, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xED};
	nDataLenRet = sizeof(databuf);

	databuf[2] = nMotorID;

	databuf[8] = GetChkSum(databuf + 2, 6);
	return databuf;
}


byte* CRobotCmd::GetCmd_MotorGetID(int& nDataLenRet)
{
	return GetCmd_MotorGetOffset(0, nDataLenRet);
}


byte* CRobotCmd::GetCmd_MotorSetID(int nOldMotorID, int nNewMotorID, int& nDataLenRet)
{
	static byte databuf[] = {0xFA, 0xAF, 0x01, 0xCD, 0x00, 0x00, 0x00, 0x00, 0x00, 0xED};
	nDataLenRet = sizeof(databuf);

	databuf[2] = nOldMotorID;
	databuf[5] = nNewMotorID;

	databuf[8] = GetChkSum(databuf + 2, 6);
	return databuf;
}


byte* CRobotCmd::GetCmd_MotorUpgradePrepare(int nMotorID, int& nDataLenRet)
{
	static byte databuf[] = {0xFC, 0xCF, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xED};
	nDataLenRet = sizeof(databuf);

	databuf[2] = nMotorID;

	databuf[8] = GetChkSum(databuf + 2, 6);
	return databuf;
}


byte* CRobotCmd::GetCmd_MotorUpgradeIng(int nMotorID, int nCurPage, byte* pUserData, int nUserDataLen, int& nDataLenRet)
{
	const int datalen = 71;
	const int framesize = 64;

	static byte databuf[datalen] = {0};
	memset(databuf, 0, sizeof(databuf));

	nDataLenRet = datalen;	
	
	byte head[2] = {0xFD, 0xDF};

	memcpy(databuf, head, 2);
	databuf[2] = nMotorID;
	databuf[3] = 0x01;
	databuf[4] = nCurPage;

	int nCopyLen = nUserDataLen <= framesize ? nUserDataLen:framesize;
	memcpy(databuf + 5, pUserData, nCopyLen);

	for(int i = 2; i <= datalen-3; i++)
	{
		databuf[datalen-2] += databuf[i];
	}

	databuf[datalen-1] = 0xED;

	return databuf;
}


byte* CRobotCmd::GetCmd_MotorUpgradeEnd(int nMotorID, int& nDataLenRet)
{
	const int datalen = 71;
	static byte databuf[datalen] = {0};
	memset(databuf, 0, sizeof(databuf));

	nDataLenRet = datalen;	

	byte head[2] = {0xFD, 0xDF};

	memcpy(databuf, head, 2);
	databuf[2] = nMotorID;
	databuf[3] = 0x02;
	databuf[4] = 0x00;

	for(int i = 2; i <= datalen-3; i++)
	{
		databuf[datalen-2] += databuf[i];
	}

	databuf[datalen-1] = 0xED;

	return databuf;
}


byte* CRobotCmd::GetCmd_MotorMove(int nMotorID, int nAngle, int nTime, int& nDataLenRet)
{
	static byte databuf[] = {0xFA, 0xAF, 0x01, 0x01, 0, 0x01, 0, 0x01, 0x00, 0xED};
	nDataLenRet = sizeof(databuf);

	databuf[2] = nMotorID;
	databuf[4] = nAngle;
    databuf[5] = nTime; //运行时间

    byte tempdata[2] = {0};
    memcpy(tempdata, &nTime, 2);
    databuf[6] = tempdata[1];
    databuf[7] = tempdata[0];

	databuf[8] = GetChkSum(databuf + 2, 6);

	return databuf;
}


byte* CRobotCmd::GetCmd_MotorGetState(int nMotorID, int& nDataLenRet)
{
	static byte databuf[] = {0xEF, 0xFE, 0, 0, 0, 0, 0, 0, 0, 0xED };
	nDataLenRet = sizeof(databuf);
	return databuf;
}


byte* CRobotCmd::GetCmd_RobotDebug(MOTOR_DATA* pMotorBuf, int nMotorCount, int nRunTime, int nAllTime, int& nDataLenRet)
{
	const int nMaxMotorCount = 20; //最大舵机数量
	if(nMotorCount > nMaxMotorCount)
	{
		nDataLenRet = 0;
		return NULL;
	}

	const int nMoveDataLen = nMotorCount + 2;
	int* pMoveData = new int[nMoveDataLen];

	for(int i = 0; i < nMotorCount; i++)
	{
		pMoveData[i] = pMotorBuf[i].nAngle;
	}
	pMoveData[nMotorCount] = nRunTime;
	pMoveData[nMotorCount + 1] = nAllTime;

	static byte databuf[33] = 
	{
		0xFB, 0xBF, 0x01, 0, 0, 0, 0, 0, 
		0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x3C, 0x4C, 0x6E, 
		0x5A, 0x5A, 0x78, 0x68, 0x46, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 
		0x32, 0, 0x32, 0x6D, 0xED
	};

	nDataLenRet = sizeof(databuf);

	for (int i = 0; i < nMotorCount; i++)
	{
		databuf[8+i] = pMoveData[i];
	}

	databuf[28] = pMoveData[nMotorCount] / 20;

	if (pMoveData[nMotorCount+1] >= 60)
	{
		pMoveData[nMotorCount+1] = pMoveData[nMotorCount+1] - 40;
	}

	if ((pMoveData[nMotorCount+1] / 20) < 256)
	{
		databuf[29] = 0;
		databuf[30] = pMoveData[nMotorCount+1] / 20;

		if (databuf[30] == 256)
		{
			databuf[29] = databuf[29] + 1;
			databuf[30] = 0x00;
		}
		else
		{
			if(databuf[30] == 0x00)
			{
				databuf[30] = 1;
			}
		}	
	}
	else // 暂时
	{
		databuf[29] = (pMoveData[nMotorCount+1] / 20)/256;
		databuf[30] = (pMoveData[nMotorCount+1] / 20)%256;

		if (databuf[30] == 256)
		{
			databuf[29] = databuf[29] + 1;
			databuf[30] = 0x00;
		}
		else
		{
			if(databuf[30] == 0x00)
			{
				databuf[30] = 1;
			}
		}
	}

	// 校验位
	byte chksum = 0;
	for (int n = 2; n < 31; n++)
	{
		chksum += databuf[n];
	}
	databuf[31] = chksum;

	SAFE_DELETE_ARRAY(pMoveData);

	return databuf;
}


byte* CRobotCmd::GetCmd_SwitchMode(int nMode, int& nDataLenRet)
{
    static byte databuf[] = {0xF9, 0x9F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xED};
    databuf[2] = nMode;
    databuf[8] = GetChkSum(databuf + 2, 6);
	nDataLenRet = sizeof(databuf);
	return databuf;
}


byte* CRobotCmd::GetCmd_ConnectRobot(int& nDataLenRet)
{
	static byte databuf[] = {0xF4, 0x4F, 0x01, 0x01, 0, 0, 0, 0, 0, 0xED};
	nDataLenRet = sizeof(databuf);

	databuf[8] = GetChkSum(databuf + 2, 6);
	return databuf;
}


byte* CRobotCmd::GetCmd_DisconnectRobot(int& nDataLenRet)
{
	static byte databuf[] = {0xF4, 0x4F, 0x01, 0x00, 0, 0, 0, 0, 0, 0xED};

	nDataLenRet = sizeof(databuf);
	databuf[8] = GetChkSum(databuf + 2, 6);
	return databuf;
}


byte* CRobotCmd::GetCmd_RobotReset(MOTOR_DATA* pMotorBuf, int nMotorCount, int nRunTime, int nAllTime, int& nDataLenRet)
{
	const int nMaxMotorCount = 20; //最大舵机数量
	if(nMotorCount > nMaxMotorCount)
	{
		nDataLenRet = 0;
		return NULL;
	}	

	static byte databuf[33] = 
	{
		0xFB, 0xBF, 0x01, 0, 0, 0, 0, 0, 
		0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x3C, 0x4C, 0x6E, 
		0x5A, 0x5A, 0x78, 0x68, 0x46, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 
		0x32, 0, 0x32, 0x6D, 0xED
	};

	nDataLenRet = sizeof(databuf);

	for (int i = 0; i < nMotorCount; i++)
	{
		databuf[8+i] = pMotorBuf[i].nAngle;
	}

//    databuf[28] = nRunTime / 20;

//    if (nAllTime >= 60)
//    {
//        nAllTime = nAllTime - 40;
//    }

//    if ((nAllTime / 20) < 256)
//    {
//        databuf[29] = 0;
//        databuf[30] = nAllTime / 20;

//        if (databuf[30] == 256)
//        {
//            databuf[29] = databuf[29] + 1;
//            databuf[30] = 0x00;
//        }
//        else
//        {
//            if(databuf[30] == 0x00)
//            {
//                databuf[30] = 1;
//            }
//        }
//    }
//    else // 暂时
//    {
//        databuf[29] = (nAllTime / 20)/256;
//        databuf[30] = (nAllTime / 20)%256;

//        if (databuf[30] == 256)
//        {
//            databuf[29] = databuf[29] + 1;
//            databuf[30] = 0x00;
//        }
//        else
//        {
//            if(databuf[30] == 0x00)
//            {
//                databuf[30] = 1;
//            }
//        }
//    }

    databuf[28] = nRunTime;

    byte tempdata[2] = {0};
    memcpy(tempdata, &nAllTime, 2);
    databuf[29] = tempdata[1];
    databuf[30] = tempdata[0];

	// 校验位
	byte chksum = 0;
	for (int n = 2; n <= 30; n++)
	{
		chksum += databuf[n];
	}
	databuf[31] = chksum;

	//SAFE_DELETE_ARRAY(pMoveData);

	return databuf;
}


byte* CRobotCmd::GetCmd_RobotReadSN(int& nDataLenRet)
{
#if 1
	static byte databuf[] = {0xFB, 0xBF, 0x00, 0x33, 0x00, 0, 0xED};

	nDataLenRet = sizeof(databuf);
	databuf[2] = nDataLenRet - 1;

	databuf[nDataLenRet - 2] = GetChkSum(databuf + 2, nDataLenRet - 4);
	return databuf;
#else
	return NULL;
#endif
}


byte* CRobotCmd::GetCmd_RobotWriteSN(char* pszSN, int nSNlen, int& nDataLenRet)
{
#if 1
	static byte databuf[] = {0xFB, 0xBF, 0x00, 0x33, 0x01, 
		0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0xED};

	nDataLenRet = sizeof(databuf);

	databuf[2] = nDataLenRet - 1;

	if(nSNlen > 16)
		nSNlen = 16;
	memcpy(databuf + 5, pszSN, nSNlen);

	databuf[nDataLenRet - 2] = GetChkSum(databuf + 2, nDataLenRet - 4);
	return databuf;
#else
	return NULL;
#endif
}


byte* CRobotCmd::GetCmd_RobotReadUID(int& nDataLenRet)
{
#if 1
	static byte databuf[] = {0xFB, 0xBF, 0x00, 0x34, 0x00, 0, 0xED};

	nDataLenRet = sizeof(databuf);
	databuf[2] = nDataLenRet - 1;

	databuf[nDataLenRet - 2] = GetChkSum(databuf + 2, nDataLenRet - 4);
	return databuf;
#else
	return NULL;
#endif
}


byte* CRobotCmd::GetCmd_RobotCheckUTF8(int& nDataLenRet)
{
#if 1
	static byte databuf[] = {0xFB, 0xBF, 0x00, 0x35, 0x00, 0, 0xED};

	nDataLenRet = sizeof(databuf);
	databuf[2] = nDataLenRet - 1;

	databuf[nDataLenRet - 2] = GetChkSum(databuf + 2, nDataLenRet - 4);
	return databuf;
#else
	return NULL;
#endif
}


byte* CRobotCmd::GetCmd_MotorGetAngle(int nMotorID, int& nDataLenRet)
{
    static byte databuf[] = {0xFA, 0xAF, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xED};

    nDataLenRet = sizeof(databuf);
    databuf[2] = nMotorID;
    databuf[nDataLenRet - 2] = GetChkSum(databuf + 2, nDataLenRet - 4);
    return databuf;
}
