

#ifndef CPHONENUMREGEXP_H
#define CPHONENUMREGEXP_H


#include "cfbuibase_global.h"
#include <QLineEdit>

class CFBUIBASE_EXPORT CPhoneNumRegExp : public QLineEdit
{
public:    
    explicit CPhoneNumRegExp(QWidget *parent = 0);
    virtual ~CPhoneNumRegExp();

public:

};

#endif // CPHONENUMREGEXP_H
