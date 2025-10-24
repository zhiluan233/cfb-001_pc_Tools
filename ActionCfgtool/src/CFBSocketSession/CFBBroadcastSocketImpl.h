#ifndef DEVMONITOR_H
#define DEVMONITOR_H

#include <QObject>
#include "robotdef.h"
#include "Header.h"
#include "CFBBroadcastSocket.h"

#define BROADCAST_PORT   6000

QT_BEGIN_NAMESPACE
class QUdpSocket;
QT_END_NAMESPACE

class CCFBBroadcastSocketImpl : public QObject, public CICFBBroadcastSocket
{
    Q_OBJECT
public:
    explicit CCFBBroadcastSocketImpl(QObject *parent = 0);
    virtual ~CCFBBroadcastSocketImpl();

signals:
    void showDev(DevInfo &dev);

private slots:
    void processPendingDatagrams();
    void regDev(DevInfo &dev);

public:
    virtual bool Initial(HWND m_hWnd,ICFBBroadcastEvent* pEvent);
    virtual bool Release(void);

private:
    int ParseHeaderData(char * pData, int nLen, CHeader &header);
    void ParseMsg(char* pMsg , int nLen);
    bool SendMsg(int nId , const std::string& msgBody);

private:
    QUdpSocket *udpSocket;
    QList<QString>  devLst;
    ICFBBroadcastEvent *m_pEvent;
};

#endif // QDEVMONITOR_H
