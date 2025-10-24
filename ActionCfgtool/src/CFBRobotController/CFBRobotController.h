// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 CFBROBOTCONTROLLER_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// CFBROBOTCONTROLLER_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。

#ifndef CFBROBOTCONTROLLER_H
#define CFBROBOTCONTROLLER_H

#include <QtCore/qglobal.h>

#ifdef CFBROBOTCONTROLLER_EXPORTS
#define CFBROBOTCONTROLLER_API Q_DECL_EXPORT
#else
#define CFBROBOTCONTROLLER_API Q_DECL_IMPORT
#endif

//#include <Dbt.h>
//#include <SetupAPI.h>
#include <QFile>
#include "RobotController.h"

#ifdef Q_OS_WIN
CFBROBOTCONTROLLER_API extern CRobotController*   g_pRobotController;  //机器人通信控制器
#else
extern CRobotController*   g_pRobotController;  //机器人通信控制器
#endif


CFBROBOTCONTROLLER_API CRobotController* CreateRobotController(QWidget* parent = NULL);


CFBROBOTCONTROLLER_API void ReleaseRobotController();


#endif
