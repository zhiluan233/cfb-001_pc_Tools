#include "cfbthread.h"

#ifdef _DEBUG
#include <QDebug>
#endif

CFBThread::CFBThread() :
m_thread(NULL),
    m_bRun(false),
    m_bStopView(false)
{

}
CFBThread::~CFBThread()
{
    if(m_thread)
    {
        delete m_thread;
        m_thread = NULL;
    }
}

bool CFBThread::StartThread()
{
   if(!m_thread)
   {
       m_thread = new CFBThread();
//       connect(CFBThread, &CFBThread::resultReady, this, &MyObject::handleResults);
//       connect(CFBThread, &CFBThread::finished, m_thread, &QObject::deleteLater);
//       workerThread->start();
   }
   return true;
}

void CFBThread::run()
{

//    m_bRun = true;
//    while (!GetFileLoadStatus())
//    {
//        sleep(10);
//    }

//    while(true)
//    {
//        if(m_bStopView)
//        {
//             qDebug()<<"Exit 3d view"<<endl;
//             break;
//        }
//        else
//        {
//             glutMainLoopEvent();
//             sleep(100);
//        }
//    }
}

void CFBThread::ThreadMainLoop()
{

//    glutMainLoopEvent();
}
