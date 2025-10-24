#pragma once
#include "ICFBSocketSession.h"
#include <QHostAddress>
#include <QTcpSocket>
#include <QThreadPool>
#include <QWaitCondition>
#include <QMutex>
#include "type.h"

class QMutex;
class QTimer;
class CDataSend
{
public:
	char* m_pDataToSend;
	int m_nDataLen;

public:
	bool SetData(const char* pData, int nLen);
};

class CCFBSocketSessionImpl;
class CProcessSendData: public QRunnable
{
public:
    CProcessSendData();
    ~CProcessSendData();

    bool Connection(CCFBSocketSessionImpl* pSession);
    bool ReleaseConnection();

private:
    bool					m_bStop;
    CCFBSocketSessionImpl*	m_pSession;

public:
    
    void run();
};

class CFBSOCKETSESSIONSHARED_EXPORT CCFBSocketSessionImpl: public QObject, public CICFBSocketSession
{
    Q_OBJECT
public:
    explicit CCFBSocketSessionImpl(QObject *parent = 0);
	~CCFBSocketSessionImpl(void);

public:
    void run();
    virtual bool Connect(QString strServerIPAddress, int nTimeOutSec,  ICFBSocketSessionEvent* pEvent);
    virtual bool StopConnect();
    virtual void ReleaseConnection();
    virtual bool SendData(const char* pData, int nLen);
    virtual bool SendObj(int nId, QObject* pObj);

    virtual bool SendMsg(int nId , const std::string& msgBody);

    virtual bool CFBSocketSession_SetUploadFileInfo(QString strFilePath , QString strFilename, int nFileLen , short nType, QString strMD5 = "");
    virtual bool CFBSocketSession_QueryVersion(int nType);

    // 胸板升级
    virtual bool CFBSocketSession_UpgradeChestReq(int nType, int nFileSize);
    virtual bool CFBSocketSession_WriteChestData(int nPageNum, char* pData , int nDataLen);
    virtual bool CFBSocketSession_WriteCheckCode(const char* chMd5 , int nMd5Len);
//    virtual bool CFBSocketSession_EnterFastBootMode();

    virtual bool CFBSocketSession_GetExportFileSource(QString strFilename);
    virtual bool CFBSocketSession_GetDetailFileInfo(QString strFilename);

    virtual void CFBSocketSession_GetActionList();
    virtual void CFBSocketSession_PlayActionFile(QString strFileName);
    virtual void CFBSocketSession_DeleteActionFile(QString strFileName);
    virtual void CFBSocketSession_SetMotorRotation(int nID, int nAngle, int iTime);
    virtual void CFBSocketSession_SetAllAngleToAlpha(MOTOR_DATA* pMotor, int nMotorCount, int nTime, int nCurveType);
    virtual void CFBSocketSession_ReadMotorRotation(int nID, bool bPowerDown);
    virtual void CFBSocketSession_StopPlay();
    virtual void CFBSocketSession_PlayLedData(int type,byte leftLed, byte rightLed, byte bright, byte color, int runTime, int lightUpTime, int lightDownTime, bool bEnd);
    virtual void CFBSocketSession_StopPlayLedData(int type);
    virtual void CFBSocketSession_PlayExpression(QString strFileName, int nCurFrame);
    virtual void CFBSocketSession_ConnectToRobot(int nToRobotId);
    virtual void CFBSocketSession_DownloadApkToRobot(QString downloadUrl);
    virtual void CFBSocketSession_UninstallApkFromRobot(QString apkName);
    virtual void CFBSocketSession_GetInstallApkList();
    virtual void CFBSocketSession_SetChargeAndPlay(bool bOpen);
    //获取窗体
    virtual QObject* GetObject();

public:
	bool ProcessSendData();

public slots:
  void onTimerOut();
  void readForReady();
  void displayError(QAbstractSocket::SocketError socketError);
  void tcpConnected();
  void tcpDisconnected();
  void slotMotionPlay(MOTOR_DATA* pMotor, int nMotorCount, int nRunTime, int nCurveType);

signals:
    void sigRobotConnected();
    void sigRobotDisconnected();

private:
	void ClearDataSendList();
	void AddDataToSendList(CDataSend* pSend);
	// 返回 -1 Socket错误
	int ReadFromSocket();
	void ParseMsg(char* pMsg, int nLen);
	// 发送心跳包
	void SendXT();
	bool SendLoginCmd(QString strPassword);
	
private:
	bool				m_bStop;
    QTcpSocket			*m_Socket;
	int					m_nTimeOutSec;	
	//HWND				m_hNotifyWnd;
    std::string         m_strServerIPAddress;
    QList<CDataSend*>	m_listDataToSend;
	bool				m_bConnectToServer;
//    CProcessSendData	m_ProcessSendData;
	long				m_lLastRcvDataTime;		// 上一次接收到数据的时间，用于超时用
	long				m_lLastSendDataTime;	// 最后一次发送数据的时间
    long                m_nSerialNum;

    int                 m_nReadTotalLen;        // 待接收数据总长度
    int                 m_nRecvDataLen;         // 已接收数据长度
    char*               m_pchRecvBuf;           // 接收数据缓存区

    CProcessSendData*	m_ProcessSendData;
    QMutex              m_mutex;
    QString             m_sendFilePath;
	ICFBSocketSessionEvent*	m_pEvent;
    QTimer *m_MyTimer;
    QMutex m_oMutex;
    QWaitCondition m_oWait;
	
public:
	bool IsConnected(void);
};

