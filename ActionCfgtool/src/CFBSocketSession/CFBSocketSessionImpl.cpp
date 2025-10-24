#include "StdAfx.h"
#include "CFBSocketSessionImpl.h"
#include <QTimer>
#include <QMutexLocker>
#include <QMutex>
#include <QTime>
//#include "../LibPackClass/LoginAlpha.h"
//#include "../LibPackClass/Header.h"
//#include "../LibPackClass/HeaderCmd.h"
//#include "../LibPackClass/LoginResponse.h"
//#include "../LibPackClass/FileInfo.h"
//#include "../LibPackClass/AlphaServerInfo.h"
//#include "../LibPackClass/GetActionFileList.h"
//#include "../LibPackClass/WifiList.h"
//#include "../LibPackClass/SetWifiToConnect.h"
//#include "../LibPackClass/PlayActionFile.h"
//#include "../LibPackClass/MotoAngle.h"
//#include "../LibPackClass/ReadMotorAngle.h"
//#include "../LibPackClass/DelActionFileInfo.h"
//#include "../LibPackClass/GetActionRelatedList.h"
//#include "../LibPackClass/EarLedData.h"
//#include "../LibPackClass/EyeLedData.h"
//#include "../LibPackClass/GetAlphaVersionInfo.h"
//#include "../LibPackClass/AllMotoAngle.h"
#include "constant.h"
#include "configs.h"
#include "Header.h"
#include "protocoldef.h"
#include "AlphaMessage.pb.h"
#include "AlphaMessageHeader.pb.h"
#include "CmGetActionList.pb.h"
#include "CmNewActionInfo.pb.h"
#include "CmPlayActionFile.pb.h"
#include "AlPlayActionCommand.pb.h"
#include "AlStopPlayAction.pb.h"
#include "AlDeleteActionFile.pb.h"
#include "CmBaseResponse.pb.h"
#include "CmLoginAlpha.pb.h"
#include "CmReadMotorAngle.pb.h"
#include "AlPcSetMotorAngle.pb.h"
#include "CmSetChargeAndPlayData.pb.h"
// 新增
#include "AlPcSetAllMotorsAngle.pb.h"
#include "AlPcSetExpressFrame.pb.h"
#include "AlPcEmulatingLed.pb.h"
#include "AlPcStopEmulatingLed.pb.h"
#include "AlPcTransferFile.pb.h"
#include "AlPcExportFile.pb.h"
#include "AlPcGetDetailFile.pb.h"
#include "AlPcGetVersion.pb.h"
#include "AlPcChestUpgrade.pb.h"

bool CreateCFBSocketSession(CICFBSocketSession** ppSession)
{
	CCFBSocketSessionImpl* pProxy = new CCFBSocketSessionImpl();
	if (pProxy)
	{
		*ppSession = pProxy;
		return true;
	}

	return false;
}

bool CDataSend::SetData(const char* pData, int nLen)
{
	if (pData && nLen)
	{
		m_pDataToSend = new char[nLen];
		memcpy(m_pDataToSend, pData, nLen);
		m_nDataLen = nLen;
		return true;
	}

	return false;
}

CProcessSendData::CProcessSendData()
{
    m_pSession = NULL;
    m_bStop = false;
}

CProcessSendData::~CProcessSendData()
{
    m_bStop = true;
}

bool CProcessSendData::Connection(CCFBSocketSessionImpl* pSession)
{
    m_pSession = pSession;
    m_bStop = false;

//    start();
//    QThreadPool::globalInstance()->start(this);

    return true;
}

bool CProcessSendData::ReleaseConnection()
{
    m_bStop = true;
    return true;
}


void CProcessSendData::run()
{
    if (m_pSession == NULL)
        return;

    while(m_bStop == false)
    {
        if (!m_pSession->ProcessSendData())
           QThread::msleep(1);
    }
}

CCFBSocketSessionImpl::CCFBSocketSessionImpl(QObject *parent)
    : QObject(parent)
    //, m_hNotifyWnd(NULL)
	, m_bStop(true)
	, m_nTimeOutSec(30)
	, m_bConnectToServer(false)
	, m_pEvent(NULL)
    , m_MyTimer(NULL)
    , m_ProcessSendData(NULL)
    , m_nSerialNum(0)
    , m_nReadTotalLen(0)
    , m_nRecvDataLen(0)
    , m_pchRecvBuf(NULL)
{
    m_strServerIPAddress = "";
}

CCFBSocketSessionImpl::~CCFBSocketSessionImpl(void)
{
    delete m_Socket;
    delete m_MyTimer;
}

//获取窗体
QObject* CCFBSocketSessionImpl::GetObject()
{
    return this;
}

void CCFBSocketSessionImpl::run()
{
    while(m_bStop == false)
    {
        if (!ProcessSendData())
            QThread::msleep(100);

//        m_oMutex.lock();
//        ProcessSendData();
//        sleep(1);
//        m_oWait.wait(&m_oMutex, 1000000);
//        m_oMutex.unlock();
    }

    m_Socket->abort();
}


bool CCFBSocketSessionImpl::Connect(QString strServerIPAddress, int nTimeOutSec, ICFBSocketSessionEvent* pEvent)
{
//	ReleaseConnection();
//    m_ProcessSendData.ReleaseConnection();
	m_bStop = true;
//	WaitForStop();

	ClearDataSendList();

    m_strServerIPAddress = strServerIPAddress.toStdString();
//	strcpy(m_strServerIPAddress, strServerIPAddress);
	m_nTimeOutSec = nTimeOutSec;
	//m_hNotifyWnd = hWnd;
	m_bStop = false;
	m_pEvent = pEvent;

//    m_ProcessSendData.Connection(this);
//    m_ProcessSendData.run();
//	return StartThread();

    m_Socket = new QTcpSocket(this);

    connect(m_Socket, SIGNAL(connected()), this, SLOT(tcpConnected()));
    connect(m_Socket, SIGNAL(disconnected()), this, SLOT(tcpDisconnected()));
    connect(m_Socket, SIGNAL(readyRead()), this, SLOT(readForReady()));
    connect(m_Socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

    m_Socket->connectToHost(QHostAddress(strServerIPAddress), 6100);
    //m_Socket->setSocketOption(QAbstractSocket::LowDelayOption , "1");
    m_Socket->setSocketOption(QAbstractSocket::KeepAliveOption,"1");

    m_MyTimer = new QTimer();
    connect(m_MyTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()));

//    m_ProcessSendData = new CProcessSendData;
//    m_ProcessSendData->Connection(this);
//    QThreadPool::globalInstance()->start(m_ProcessSendData);

    return true;
}


void CCFBSocketSessionImpl::onTimerOut()
{
    SendXT();
}


void CCFBSocketSessionImpl::readForReady()
{
    ReadFromSocket();
}


void CCFBSocketSessionImpl::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        break;
    case QAbstractSocket::ConnectionRefusedError:
        break;
    default:
        break;
    }
    QString strErr = m_Socket->errorString();
    //AddLog(strErr);
    qDebug()<<"The following error occurred:"<<strErr<<endl;
}


void CCFBSocketSessionImpl::tcpConnected()
{
    // Modify by jianjie 2017/6/29 
    // note:socket连接成功即已连接到服务器，若等待login回复，则不能立即操作socket
    m_bConnectToServer = true;

    SendLoginCmd("");
    m_MyTimer->start(3000);
    emit sigRobotConnected();
}


void CCFBSocketSessionImpl::tcpDisconnected()
{
    m_bConnectToServer = false;

    if (m_MyTimer)
    {
        m_MyTimer->stop();
    }

    emit sigRobotDisconnected();
}


void CCFBSocketSessionImpl::ReleaseConnection()
{
    m_Socket->abort();

    if (m_MyTimer)
    {
        delete m_MyTimer;
        m_MyTimer = NULL;
    }

	m_bStop = true;
	//WaitForStop();

	ClearDataSendList();
    if (m_ProcessSendData)
    {
        m_ProcessSendData->ReleaseConnection();
        // 从线程池中移除
        QThreadPool::globalInstance()->cancel(m_ProcessSendData);
        QThreadPool::globalInstance()->waitForDone();
        QThreadPool::globalInstance()->clear();
    }

	delete this;
}




bool CCFBSocketSessionImpl::StopConnect()
{
	m_bStop = true;
	ClearDataSendList();

    m_Socket->disconnectFromHost();
    if (m_ProcessSendData)
    {
        m_ProcessSendData->ReleaseConnection();
        // 从线程池中移除
        QThreadPool::globalInstance()->cancel(m_ProcessSendData);
        QThreadPool::globalInstance()->waitForDone();
        QThreadPool::globalInstance()->clear();
    }

	return true;
}


bool CCFBSocketSessionImpl::SendData(const char* pData, int nLen)
{
//    CFBAutoLock l(&m_myCriSec);
//    if (pData == NULL)
//        return false;

//    CDataSend* pSend = new CDataSend();
//    if (pSend == NULL)
//        return false;

//    if (pSend->SetData(pData, nLen) == false)
//    {
//        delete pSend;
//        return false;
//    }
//    AddDataToSendList(pSend);

//    m_Socket->write(pData, nLen);

    int nSendDataLen = 0;
    while(nSendDataLen < nLen )
    {
        nSendDataLen += m_Socket->write(pData + nSendDataLen, nLen - nSendDataLen);
        //m_Socket->flush();
        if(-1 == nSendDataLen)
        {
            emit sigRobotDisconnected();
            return false;
        }
    }

	return true;
}



bool CCFBSocketSessionImpl::SendObj(int nId, QObject* pObj)
{

	return true;
}

// protobuf发送消息
bool CCFBSocketSessionImpl::SendMsg(int nId , const std::string& msgBody)
{
    if (m_bConnectToServer == false && nId != ALPHA_MSG_LOGIN)
        return false;

    AlphaMessage sendMsg;

    AlphaMessageHeader* msgHeader = sendMsg.mutable_header();
    msgHeader->set_commandid(nId);
    msgHeader->set_versioncode("1");
    msgHeader->set_sendserial(++m_nSerialNum);
    msgHeader->set_responseserial(0);

    sendMsg.set_bodydata(msgBody);

    std::string outdata = sendMsg.SerializeAsString();

    CHeader header;
    header.SetMsg(outdata.c_str(), outdata.size());

    char* pData = NULL;
    int nLen = 0;
    nLen = header.GetHeaderData(&pData);
    if (pData && nLen)
    {
        SendData(pData, nLen);
        delete [] pData;
    }

    sendMsg.clear_header();

    return true;
}


int CCFBSocketSessionImpl::ReadFromSocket()
{
    // 消息读取完毕则取头
    if (m_nReadTotalLen == m_nRecvDataLen)
    {
        CHeader header;
        //接收包标志
        int nRecv = 0;
        if ((nRecv = m_Socket->read((char*)&header.m_nFlag,sizeof(header.m_nFlag))) != sizeof(header.m_nFlag))
        {
            return -1;
        }
        if (header.m_nFlag != ALPHA_SOCKET_FLAG)
            return 0;
        // 取出包长度
        if (m_Socket->read((char*)&header.m_nLength, sizeof(header.m_nLength)) != sizeof(header.m_nLength))
            return -1;
        // 长度不能小于4个字节
        if (header.m_nLength < 4)
            return 0;
        // 取出协议版本
        if (m_Socket->read((char*)&header.m_nVersion, sizeof(header.m_nVersion)) != sizeof(header.m_nVersion))
            return -1;
        if (header.m_nVersion != ALPHA_SOCKET_VERSION)
            return 0;
        // 取出辅助信息
        if (m_Socket->read((char*)&header.m_nExtInfo, sizeof(header.m_nExtInfo)) != sizeof(header.m_nExtInfo))
            return -1;
        // 心跳包
        if (header.m_nLength == 4)
            return 1;

        m_nReadTotalLen     = header.m_nLength - 4;
        m_nRecvDataLen      = 0;
        m_pchRecvBuf        = new char[m_nReadTotalLen];
    }

    // 持续读取消息，直至取得全部数据
    if (m_nRecvDataLen != m_nReadTotalLen)
    {
        m_nRecvDataLen += m_Socket->read(m_pchRecvBuf + m_nRecvDataLen , m_nReadTotalLen - m_nRecvDataLen);

        if (m_nRecvDataLen == m_nReadTotalLen)
        {
            ParseMsg(m_pchRecvBuf , m_nReadTotalLen);

            delete [] m_pchRecvBuf;
            m_pchRecvBuf = NULL;

            ReadFromSocket();
        }
    }

    return 1;

 //   // 取出消息内容
 //   int nLenRead = header.m_nLength - 4;
 //   header.m_pMsg = new char[nLenRead];
 //   int nRecvLen = m_Socket->read(header.m_pMsg, nLenRead);
 //   //m_Socket->readAll()
 //   if (nRecvLen != nLenRead)
 //   {
 //       nRecvLen += m_Socket->read(header.m_pMsg + nRecvLen , nLenRead - nRecvLen);
 //       if (nRecvLen == nLenRead)
 //       {
 //           int nTemp = 0;
 //       }
 //       delete [] header.m_pMsg;
 //       header.m_pMsg = NULL;

 //       return -1;
 //   }

	//ParseMsg(header.m_pMsg, nLenRead);

	//delete [] header.m_pMsg;
	//header.m_pMsg = NULL;

	//return 1;
}

void CCFBSocketSessionImpl::ParseMsg(char* pMsg, int nLen)
{
    AlphaMessage recvMsg;
    recvMsg.ParseFromArray(pMsg , nLen);

    //QString strMsg;
    //for (int i = 0; i < nLen; ++i)
    //{
    //    QString tmp = QString::number((int)(pMsg[i] & 0xff), 16).toUpper();
    //    bool ok = false;
    //    if ((tmp.toInt(&ok, 16) < 0x10))
    //    {
    //        tmp = tmp.insert(0, "0");
    //    }
    //    strMsg += tmp;
    //}

    //qDebug() << strMsg << endl;

    AlphaMessageHeader msgHeader = recvMsg.header();
    std::string msgBody = recvMsg.bodydata();

    int nCommandId = msgHeader.commandid();
    switch (nCommandId)
    {
    case RSP_DELETE_ACTION_FILES:           // 删除动作文件成功则刷新动作列表
        {
            AlDeleteActionFileResponse deleteFileRsp;
            deleteFileRsp.ParseFromString(msgBody);
            bool bResult = deleteFileRsp.issuccess();
            if (bResult)
            {
                CFBSocketSession_GetActionList();
            }
        }
        break;
    case RSP_GET_NEW_ACTION_LIST:           // 获取动作列表
        {
            CmGetActionListResponse actionList;
            actionList.ParseFromString(msgBody);

            QStringList fileInfo;

            for (int i = 0; i < actionList.actionlist_size(); ++i)
            {
                CmNewActionInfoResponse action = actionList.actionlist(i);
                fileInfo.append(QString::fromStdString(action.actionid()));
            }

            if (m_pEvent)
            {
                m_pEvent->OnCFBSocketSessionEvent_ActionFileList(fileInfo);
            }
        }
        break;
    case RSP_GET_MOTOR_ANGLE:               // 读取舵机角度
        {
            CmReadMotorAngleResponse readAngleRsp;
            readAngleRsp.ParseFromString(msgBody);

            int nServoId = readAngleRsp.motorid();
            int nAngel   = readAngleRsp.angle();

            qDebug() << "motor id:" << nServoId << "angel:" << nAngel << endl;

            if (m_pEvent)
            {
                m_pEvent->OnCFBSocketSessionEvent_ReadMotorAngle(nServoId , nAngel);
            }
        }
        break;
    case RSP_TRANS_FILE_RESOURCE:           // 导入文件
        {
            AlPcTransferFileResponse transferFileRsp;
            transferFileRsp.ParseFromString(msgBody);
            int     nPort       = transferFileRsp.nport();
            QString strIp       = QString::fromStdString(transferFileRsp.strip());
            QString strFileName = QString::fromStdString(transferFileRsp.strfilename());

            if (m_pEvent)
            {
                m_pEvent->OnCFBSocketSessionEvent_ImportFile(nPort , strIp , strFileName);
            }
        }
        break;
    case RSP_EXPORT_FILE_RESOURCE:          // 获取导出文件对应资源
        {
            AlPcExportFileResponse exportFileRsp;
            exportFileRsp.ParseFromString(msgBody);

            bool bGetNextFile = false;

            for (int i = 0; i < exportFileRsp.fileinfolist_size(); ++i)
            {
                AlPcExportFileInfo exportFileInfo = exportFileRsp.fileinfolist(i);
                QString strFileName = QString::fromStdString(exportFileInfo.strfilename());

                // 导出资源插入结束，获取下一资源
                if (i == exportFileRsp.fileinfolist_size() - 1)
                {
                    bGetNextFile = true;
                }

                if (m_pEvent)
                {
                    m_pEvent->OnCFBSocketSessionEvent_ExportFileSource(strFileName , exportFileInfo.nfilelen() , bGetNextFile);
                }
            }
        }
        break;
    case RSP_GET_DETAIL_FILE_INFO:          // 获取某一导出文件详细信息
        {
            AlPcTransferFileResponse transferFileRsp;
            transferFileRsp.ParseFromString(msgBody);
            int     nPort       = transferFileRsp.nport();
            QString strIp       = QString::fromStdString(transferFileRsp.strip());
            QString strFileName = QString::fromStdString(transferFileRsp.strfilename());

            if (m_pEvent)
            {
                m_pEvent->OnCFBSocketSessionEvent_ExportFile(nPort , strIp , strFileName);
            }
        }
        break;
    case RSP_GET_CHEST_VERSION:          // 读取软件版本
        {
            AlPcGetVersionResponse getVersionRsp;
            getVersionRsp.ParseFromString(msgBody);
            QString strVersion = QString::fromStdString(getVersionRsp.strversion());
        }
        break;
    case RSP_CHEST_UPGRADE:                 // 胸板升级
        {
            AlPcChestUpgradeData chestUpgradeRsp;
            chestUpgradeRsp.ParseFromString(msgBody);

            int nCmdId = chestUpgradeRsp.commandid();
            bool bSucceed = chestUpgradeRsp.success();

            qDebug() << "cmdId:" << nCmdId << "status:" << bSucceed << endl;
        }
        break;
    case RSP_SET_ALL_MOTORS_ANGLE:          // 设置所有舵机角度
        {
            CmBaseResponse baseRsp;
            baseRsp.ParseFromString(msgBody);
            int nResult = baseRsp.result();
        }
        break;
    case RSP_SET_EXPRESS_FRAME:
        {
            CmBaseResponse baseReq;
            baseReq.ParseFromString(msgBody);
            int nResult = baseReq.result();
        }
    break;
    case RSP_SET_CHARGE_AND_PLAY_DATA:      // 设置边充边玩
        {
            CmSetChargeAndPlayDataResponse chargAndPlayRsp;
            chargAndPlayRsp.ParseFromString(msgBody);

            bool bSuccess       = chargAndPlayRsp.issuccess();
            int  nResultCode    = chargAndPlayRsp.resultcode();
        }
        break;
    default:
        break;
    }
}

void CCFBSocketSessionImpl::ClearDataSendList()
{
    QMutexLocker locker(&m_mutex);
    for (int i = 0; i < m_listDataToSend.count(); i++)
	{
        CDataSend* pSend = m_listDataToSend.at(i);
//		CDataSend* pSend = m_listDataToSend.GetAt(m_listDataToSend.FindIndex(i));
		if (pSend)
		{
			if (pSend->m_pDataToSend)
			{
				delete [] pSend->m_pDataToSend;
				pSend->m_pDataToSend = NULL;
			}

			delete pSend;
		}
	}

    m_listDataToSend.clear();
}

void CCFBSocketSessionImpl::AddDataToSendList(CDataSend* pSend)
{
    m_listDataToSend.append(pSend);
}

bool CCFBSocketSessionImpl::ProcessSendData()
{
    QMutexLocker locker(&m_mutex);
    if (m_listDataToSend.count() == 0)
		return false;

    CDataSend* pSend = m_listDataToSend.first();
	if (pSend)
	{
        int nSize = m_Socket->write(pSend->m_pDataToSend, pSend->m_nDataLen);
        if (nSize == pSend->m_nDataLen)
        {
            qDebug()<<"The m_nDataLen:"<<pSend->m_nDataLen<<endl;
            delete []pSend->m_pDataToSend;
            delete pSend;

            m_listDataToSend.removeFirst();
            return true;
        }
	}
	return false;
}

// 心跳包
void CCFBSocketSessionImpl::SendXT()
{
    //SendMsg(REQ_HEARTBEAT , "");

    if (!m_bConnectToServer)
    {
        return;
    }

    CHeader head;
    head.SetMsg(NULL, 0);

    char* pData = NULL;
    int nLen = 0;
    nLen = head.GetHeaderData(&pData);
    if (pData && nLen)
    {
        SendData(pData, nLen);
        delete [] pData;
    }

}

// 登陆信息
bool CCFBSocketSessionImpl::SendLoginCmd(QString strPassword)
{
    CmLoginAlphaRequest loginReq;
    loginReq.set_mstrpassword(strPassword.toStdString());

    std::string msgBody = loginReq.SerializeAsString();

    return SendMsg(REQ_LOGIN , msgBody);
}


//bool CCFBSocketSessionImpl::CFBSocketSession_SendDownloadFileInfo(QString strFilePath ,QString strFilename, int nFileLen , short nType)
//{
//	CFileInfo_DL fileinfo;
//	//CStringA strTemp = strFilename.GetBuffer(0);
////	fileinfo.setFileName(ConvertUniodeToUtf8(strFilename)) ;
////	fileinfo.setFilePath(ConvertUniodeToUtf8(strFilePath));
////	fileinfo.setFileLen(nFileLen);

//    return SendObj(nType,(QObject*)&fileinfo);
//}



//bool CCFBSocketSessionImpl::CFBSocketSession_EnterFastBootMode()
//{
//	CAlpha2Version versionInfo;
//    return SendObj(ALPHA_MSG_ENTER_FASTBOOT_MODE,(QObject*)&versionInfo);
//}


bool CCFBSocketSessionImpl::CFBSocketSession_QueryVersion(int nType)
{
    AlPcGetVersionRequest getVersionReq;
    getVersionReq.set_ntype(nType);

    std::string msgBody = getVersionReq.SerializeAsString();

    return SendMsg(REQ_GET_CHEST_VERSION , msgBody);
}


bool CCFBSocketSessionImpl::CFBSocketSession_UpgradeChestReq(int nType, int nFileSize)
{
    int nCmdId = 0x01;

    AlPcChestUpgradeData chestUpgradeReq;
    chestUpgradeReq.set_commandid(nCmdId);
    chestUpgradeReq.set_devicetype(nType);
    chestUpgradeReq.set_filesize(nFileSize);

    std::string msgBody = chestUpgradeReq.SerializeAsString();

    return SendMsg(REQ_CHEST_UPGRADE, msgBody);
}


bool CCFBSocketSessionImpl::CFBSocketSession_WriteChestData(int nPageNum, char* pData , int nDataLen)
{
    int nCmdId = 0x02;

    AlPcChestUpgradeData writeChestData;
    writeChestData.set_commandid(nCmdId);

    UpgrdeInfo* upgradeInfo = writeChestData.mutable_upgradeinfo();
    upgradeInfo->set_pagenum(nPageNum);
    upgradeInfo->set_pagedata(pData, nDataLen);

    std::string msgBody = writeChestData.SerializeAsString();

    return SendMsg(REQ_CHEST_UPGRADE, msgBody);
}


bool CCFBSocketSessionImpl::CFBSocketSession_WriteCheckCode(const char* chMd5, int nMd5Len)
{
    int nCmdId = 0x03;

    AlPcChestUpgradeData writeCheckCode;
    writeCheckCode.set_commandid(nCmdId);
    writeCheckCode.set_md5(chMd5 , nMd5Len);

    QString strMsg;

    for (int i = 0; i < nMd5Len; ++i)
    {
        QString tmp = QString::number((int)(chMd5[i] & 0xff), 16).toUpper();
        bool ok = false;
        if ((tmp.toInt(&ok, 16) < 0x10))
        {
            tmp = tmp.insert(0, "0");
        }

        strMsg += tmp;
    }

    qDebug() << "send md5"<<strMsg << endl;

    //qDebug() << "send md5" << strMd5 <<"length:"<<strMd5.length()<< endl;

    std::string msgBody = writeCheckCode.SerializeAsString();

    return SendMsg(REQ_CHEST_UPGRADE, msgBody);
}


bool CCFBSocketSessionImpl::CFBSocketSession_SetUploadFileInfo(QString strFilePath, QString strFilename, int nFileLen, short nType, QString strMD5)
{
    m_sendFilePath = strFilePath;

    AlPcTransferFileRequest transferFileReq;
    transferFileReq.set_strfilepath(strFilePath.toStdString());
    transferFileReq.set_strfilename(strFilename.toStdString());
    transferFileReq.set_nfilelen(nFileLen);
    transferFileReq.set_ntype(nType);
    transferFileReq.set_md5value(strMD5.toStdString());

    std::string msgBody = transferFileReq.SerializeAsString();

    return SendMsg(REQ_TRANS_FILE_RESOURCE , msgBody);
}

// 请求 cfb 文件对应的资源文件
bool CCFBSocketSessionImpl::CFBSocketSession_GetExportFileSource(QString strFilename)
{
    AlPcExportFileRequest exportFileReq;
    exportFileReq.set_strfilename(strFilename.toStdString());

    std::string msgBody = exportFileReq.SerializeAsString();

    return SendMsg(REQ_EXPORT_FILE_RESOURCE , msgBody);
}

// 获取某一导出文件详细信息
bool CCFBSocketSessionImpl::CFBSocketSession_GetDetailFileInfo(QString strFilename)
{
    AlPcGetDetailFileInfoRequest getDetailInfoReq;
    getDetailInfoReq.set_strfilename(strFilename.toStdString());

    std::string msgBody = getDetailInfoReq.SerializeAsString();

    qDebug()<<"export file name:"<<strFilename<<endl;
    //AddLog("export file name:" + strFilename);

    return SendMsg(REQ_GET_DETAIL_FILE_INFO , msgBody);
}


bool CCFBSocketSessionImpl::IsConnected(void)
{
	return m_bConnectToServer;
}


void CCFBSocketSessionImpl::CFBSocketSession_GetActionList()
{
    QString strLanguageKeyword = CConfigs::getUserLanguage();
    QString strLanguage = "EN";
    if (strLanguageKeyword.compare(LANGUAGE_CHINESE_POSTFIX) == 0)
    {
        strLanguage = "CN";
    }
    else if (strLanguageKeyword.compare(LANGUAGE_ENGLISH_POSTFIX) == 0)
    {
        strLanguage = "EN";
    }

    CmGetActionListRequest actionReq;
    actionReq.set_languagetype(strLanguage.toStdString());
    actionReq.set_actiontype(1);

    std::string msgBody = actionReq.SerializeAsString();

    SendMsg(REQ_GET_NEW_ACTION_LIST , msgBody);
}


void CCFBSocketSessionImpl::CFBSocketSession_PlayActionFile(QString strFileName)
{
    AlPlayActionCommandRequest actionPlay;
    actionPlay.set_actionname(strFileName.toStdString());

    std::string msgBody = actionPlay.SerializeAsString();

    SendMsg(REQ_EXCUTE_ACTION , msgBody);
}


void CCFBSocketSessionImpl::CFBSocketSession_StopPlay()
{
    SendMsg(REQ_STOP_EXCUTING_ACTION , "");
}


void CCFBSocketSessionImpl::CFBSocketSession_DeleteActionFile(QString strFileName)
{
    AlDeleteActionFileRequest actionDelete;
    actionDelete.set_filename(strFileName.toStdString());

    std::string msgBody = actionDelete.SerializeAsString();

    SendMsg(REQ_DELETE_ACTION_FILES , msgBody);
}


void CCFBSocketSessionImpl::CFBSocketSession_SetChargeAndPlay(bool bOpen)
{
    CmSetChargeAndPlayDataRequest chargeAndPlayReq;
    chargeAndPlayReq.set_isopen(bOpen);

    std::string msgBody = chargeAndPlayReq.SerializeAsString();

    SendMsg(REQ_SET_CHARGE_AND_PLAY_DATA , msgBody);
}


void CCFBSocketSessionImpl::CFBSocketSession_SetMotorRotation(int nID, int nAngle, int nTime)
{
    AlPcSetMotorAngleRequest setAngleReq;

    setAngleReq.set_nmotorid(nID);
    setAngleReq.set_nangle(nAngle);
    setAngleReq.set_ntime(nTime);
    setAngleReq.set_runmode(0);

    std::string msgBody = setAngleReq.SerializeAsString();

    SendMsg(REQ_SET_MOTOR_ANGLE, msgBody);
}


void CCFBSocketSessionImpl::CFBSocketSession_SetAllAngleToAlpha(MOTOR_DATA* pMotor, int nMotorCount, int nTime, int nCurveType)
{
    AlPcSetAllMotorsAngleRequest setAllAngleReq;

    for (int i = 0; i < nMotorCount; ++i)
    {
        MOTOR_DATA* pTempMotor = pMotor + i;
        if (NULL == pTempMotor)
        {
            continue;
        }

        int nMotorId    = pTempMotor->nID;
        int nAngle      = pTempMotor->nAngle;

        if (-1 == nAngle)
        {
            continue;
        }

        MotorArg* motorAngel = setAllAngleReq.add_motorarg();
        motorAngel->set_id(nMotorId);
        motorAngel->set_angle(nAngle);
        motorAngel->set_runmode(nCurveType);
        motorAngel->set_runtime(nTime);
        motorAngel->set_interrupted(true);
    }

    std::string msgBody = setAllAngleReq.SerializeAsString();

    SendMsg(REQ_SET_ALL_MOTORS_ANGLE , msgBody);
}


void CCFBSocketSessionImpl::CFBSocketSession_ReadMotorRotation(int nID , bool bPowerDown)
{
    CmReadMotorAngleRequest readAngleReq;
    readAngleReq.set_motorid(nID);
    readAngleReq.set_adcump(bPowerDown);

    std::string msgBody = readAngleReq.SerializeAsString();

    SendMsg(REQ_GET_MOTOR_ANGLE , msgBody);
}



void CCFBSocketSessionImpl::CFBSocketSession_PlayLedData(int type,byte leftLed, byte rightLed, byte bright, byte color, int runTime, int lightUpTime, int lightDownTime, bool bEnd)
{
	if (m_bConnectToServer == false)
		return;

    AlPcEmulatingLedRequest emulateLedReq;
    emulateLedReq.set_type(type);
    emulateLedReq.set_leftled(leftLed);
    emulateLedReq.set_rightled(rightLed);
    emulateLedReq.set_bright(bright);
    emulateLedReq.set_color(color);
    emulateLedReq.set_runtime(runTime);
    emulateLedReq.set_lightuptime(lightUpTime);
    emulateLedReq.set_lightdowmtime(lightDownTime);
    emulateLedReq.set_bend(bEnd);

    std::string msgBody = emulateLedReq.SerializeAsString();

    SendMsg(REQ_EMULATE_LED_DATA , msgBody);
}


void CCFBSocketSessionImpl::CFBSocketSession_StopPlayLedData(int type)
{
    AlPcStopEmulatingLedRequest stopEmulateLedReq;
    stopEmulateLedReq.set_type(type);

    std::string msgBody = stopEmulateLedReq.SerializeAsString();

    SendMsg(REQ_STOP_EMULATE_LED_DATA , msgBody);
}


void CCFBSocketSessionImpl::CFBSocketSession_PlayExpression(QString strFileName, int nCurFrame)
{
    AlPcSetExpressFrameRequest setExpressFrameReq;
    setExpressFrameReq.set_name(strFileName.toStdString());
    setExpressFrameReq.set_frame(nCurFrame);

    std::string msgBody = setExpressFrameReq.SerializeAsString();

    SendMsg(REQ_SET_EXPRESS_FRAME, msgBody);

    QTime time = QTime::currentTime();
    QString strTime = time.toString("hh:mm:ss.zzz");
    qDebug() << strTime << "frame:" << nCurFrame << endl;
}

void CCFBSocketSessionImpl::CFBSocketSession_ConnectToRobot(int nToRobotId)
{
}

void CCFBSocketSessionImpl::CFBSocketSession_DownloadApkToRobot(QString downloadUrl)
{

}

void CCFBSocketSessionImpl::CFBSocketSession_UninstallApkFromRobot(QString apkName)
{

}


void CCFBSocketSessionImpl::CFBSocketSession_GetInstallApkList()
{
//	SendObj(ALPHA_MSG_GET_APPS, NULL);
}



void CCFBSocketSessionImpl::slotMotionPlay(MOTOR_DATA* pMotor, int nMotorCount, int nRunTime, int nCurveType)
{
    AlPcSetAllMotorsAngleRequest setAllAngleReq;

    for (int i = 0; i < nMotorCount; ++i)
    {
        MOTOR_DATA* pTempMotor = pMotor + i;
        if (NULL == pTempMotor)
        {
            continue;
        }

        int nMotorId    = pTempMotor->nID;
        int nAngle      = pTempMotor->nAngle;

        qDebug() << "motorId:" << nMotorId << "angel:" << nAngle << endl;

        // 角度为-1，缺省值，不进行下发
        if (-1 == nAngle)
        {
            continue;
        }

        MotorArg* motorAngel = setAllAngleReq.add_motorarg();
        motorAngel->set_id(nMotorId);
        motorAngel->set_angle(nAngle);
        motorAngel->set_runmode(nCurveType);
        motorAngel->set_runtime(nRunTime);
        motorAngel->set_interrupted(false);
    }

    std::string msgBody = setAllAngleReq.SerializeAsString();

    SendMsg(REQ_SET_ALL_MOTORS_ANGLE, msgBody);
}

