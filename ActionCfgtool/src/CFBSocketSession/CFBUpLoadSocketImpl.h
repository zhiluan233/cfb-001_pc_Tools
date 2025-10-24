
#include <QObject>
#include "ICFBTransFileSession.h"
#include<QFile>
#include <QTcpSocket>

class CCFBUpLoadSocketImpl : public QObject, public IUploadFileSession
{
    Q_OBJECT
public:
    explicit CCFBUpLoadSocketImpl(QObject *parent = 0);
    virtual ~CCFBUpLoadSocketImpl();

private slots:
    void StartReadData();
    void displayError(QAbstractSocket::SocketError socketError);

public:
    virtual bool Initial(QString strIPAddr, int nPort, QString strFileName, int nFileLen, IFileTransferEvent* pEvent);
    virtual void ReleaseConnection();

private:
    QTcpSocket *m_receiveSocket;
    IFileTransferEvent *m_pEvent;

    QString m_strFilePath;
    int m_nFileLen;
    int m_nRecvDataLen;
    QString m_strFileName;
    QFile * m_pFile;
};
