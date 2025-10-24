#pragma once
#include "cfbsocketsession_global.h"
#include "type.h"

//通知有广播包数据进入
#define ALPAHA_BROADCASTPACK_ISCOMING    1
#define ALPAHA_BROADCASTPACK_REFRESH    2

class ICFBBroadcastEvent
{
public:
	virtual void OnBroadcastEvent_DevInfo(QString strIpAddress) = 0;
};

class CICFBBroadcastSocket
{
public:
	virtual bool Initial(HWND m_hWnd,ICFBBroadcastEvent* pEvent) = 0;
	virtual bool Release(void) = 0;
};

 CFBSOCKETSESSIONSHARED_EXPORT bool CreateCFBBroadcastSocket(CICFBBroadcastSocket **ppSession);
