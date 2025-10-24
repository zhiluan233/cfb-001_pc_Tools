


#ifndef CCFBINTVALIDATOR_H
#define CCFBINTVALIDATOR_H

#include "cfbuibase_global.h"
#include <QIntValidator>

class CFBUIBASE_EXPORT CCFBIntValidator : public QIntValidator
{
    Q_OBJECT
public:
    explicit CCFBIntValidator(QString &strLastValidValue, QObject *parent = 0);
    explicit CCFBIntValidator(int minimum, int maximum, QString &strLastValidValue, QObject * parent = 0);
    ~CCFBIntValidator();

public:
    virtual void	fixup(QString & input) const override;

signals:

public slots:

private:
    QString &m_strLastValidValue;  //记录上一个合法的值，当输入的值不合法时，恢复为上一个合法的值
};

#endif // CCFBINTVALIDATOR_H
