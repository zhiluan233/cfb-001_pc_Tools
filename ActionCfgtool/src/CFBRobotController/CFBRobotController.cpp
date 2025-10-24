// CFBRobotController.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "CFBRobotController.h"

CRobotController*   g_pRobotController = NULL;  //机器人通信控制器


CFBROBOTCONTROLLER_API CRobotController* CreateRobotController(QWidget *parent)
{
    if(!g_pRobotController)
        g_pRobotController = new CRobotController(parent);
    return g_pRobotController;
}


CFBROBOTCONTROLLER_API void ReleaseRobotController()
{	
    delete g_pRobotController;
    g_pRobotController = NULL;
}
