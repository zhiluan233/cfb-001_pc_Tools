

#include "checkboxlabel.h"


const int CHECKBOX_DEFAULT_POSITOIN = 5;


CCheckBoxLabel::CCheckBoxLabel(QWidget *parent) :
    CImageLabel(parent)
{
    m_checkBox.setParent(this);
    setCheckBoxPosition(QPoint(CHECKBOX_DEFAULT_POSITOIN, CHECKBOX_DEFAULT_POSITOIN));

    connect(&m_checkBox, &QCheckBox::stateChanged, this, &CCheckBoxLabel::stateChanged);
}


CCheckBoxLabel::~CCheckBoxLabel()
{

}

void CCheckBoxLabel::setCheckBoxPosition(const QPoint &position)
{
    QRect rectVisual = this->rect();

    if(rectVisual.contains(position))
    {
        m_checkBox.move(position);
    }
    else
    {
        m_checkBox.move(CHECKBOX_DEFAULT_POSITOIN, CHECKBOX_DEFAULT_POSITOIN);
    }
}

QCheckBox *CCheckBoxLabel::getCheckBox()
{
    return &m_checkBox;
}

bool CCheckBoxLabel::isChecked() const
{
    return m_checkBox.isChecked();
}
