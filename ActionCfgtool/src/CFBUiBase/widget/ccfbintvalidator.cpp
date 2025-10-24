#include "ccfbintvalidator.h"

CCFBIntValidator::CCFBIntValidator(QString &strLastValidValue, QObject *parent) : QIntValidator(parent)
  ,m_strLastValidValue(strLastValidValue)
{

}

CCFBIntValidator::CCFBIntValidator(int minimum, int maximum, QString &strLastValidValue, QObject *parent) : QIntValidator(minimum, maximum, parent)
  , m_strLastValidValue(strLastValidValue)
{

}

CCFBIntValidator::~CCFBIntValidator()
{

}

void CCFBIntValidator::fixup(QString &input) const
{
    input = m_strLastValidValue;
}

