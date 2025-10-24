#ifndef CFBTHREAD_H
#define CFBTHREAD_H

#include <QThread>
#include "qutils_global.h"


class QUTILS_EXPORT CFBThread : public QThread
{
    Q_OBJECT
 public:
    CFBThread();
    ~CFBThread();
    bool StartThread();
  virtual   void run();
//    Q_DECL_OVERRIDE {
//        QString result;
//        emit resultReady(result);
//    }
public slots:
    void ThreadMainLoop();
 private:
    CFBThread *m_thread;
    bool m_bRun;
    bool m_bStopView;
};

#endif // CFBTHREAD_H
