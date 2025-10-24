

#ifndef CVIDEOTUTORIALHANDLE_H
#define CVIDEOTUTORIALHANDLE_H


#include <QObject>

#include "cfbupdatemodule_global.h"
#include "global.h"


class CFBUPDATEMODULESHARED_EXPORT CVideoTutorialHandle : QObject
{
    Q_OBJECT

public:
    explicit CVideoTutorialHandle();
    ~CVideoTutorialHandle();

public:
    int getUpdateCheckStatus();

};

#endif // CVIDEOTUTORIALHANDLE_H
