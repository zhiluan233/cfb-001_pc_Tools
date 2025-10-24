
#ifndef ROBOTCONTROLLER_H
#define ROBOTCONTROLLER_H

#include <QWidget>
#include <QThread>
#include "util.h"
#include "type.h"

#ifdef Q_OS_WIN
//#include <dbt.h>
#include <windows.h>
#endif

#define TIMEDELAY_CONNECTROBOT  2000   //连接机器人延时

#define FIVE_ALPHA_ROBOT_VENDOR_ID     8711
#define FIVE_ALPHA_ROBOT_PRODUCT_ID    17

#define TWO_ALPHA_ROBOT_VENDOR_ID     6353
#define TWO_ALPHA_ROBOT_PRODUCT_ID    20006

#define ALPHA_MINI_VENDOR_ID            3725
#define ALPHA_MINI_PRODUCT_ID           8221

class UsbEventThread;
class CFBROBOTCONTROLLER_API CRobotController :  public QWidget
{
        Q_OBJECT
public:	
        explicit CRobotController(QWidget* parent = NULL);
        ~CRobotController();
//#pragma region
public:

    
    void StartConnectTimer();

    
    void StartDisconncectTimer();

    
    bool TestUsbConnectRobot();

#ifdef Q_OS_WIN
        bool OnDeviceChange(MSG *msg, long *result);
#endif

private:
#if 0
        
        BOOL OpenComPort(const QString& sComName, int nBandrate, char* lpszErrStr);
#endif
        
        void RegisterDeviceNotify();

#ifdef Q_OS_WIN
        
        //void UpdateDevice(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, WPARAM wParam);
#endif

    
#if 0
    LRESULT OnDeviceChange(WPARAM wParam, LPARAM lParam);
#endif
#ifdef Q_OS_WIN
    //virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#endif
    //virtual void timerEvent(QTimerEvent *event);
    //virtual void customEvent(QEvent *event);

signals:
    // 机器人连接状态信号
    void SigConnectRobotState(bool bConnect);
private:
#ifdef Q_OS_MAC
//    static void OnDeviceArrival(void *context, IOReturn result, void *sender, IOHIDDeviceRef device);
//    static void OnDeviceRemoval(void *context, IOReturn result, void *sender, IOHIDDeviceRef device);
    
    //static int LIBUSB_CALL hotplug_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data);
#endif
//#pragma endregion
private:
    int                             m_nCurMotorID;   //当前操作舵机ID
    BOOL                            m_bSupportUTF8;  //是否支持UTF8
    UsbEventThread                  *m_pMacUsbEventThread;           // Mac平台USB热插拔监控
    static CRobotController         *m_pControllerStatic;
};
#endif

