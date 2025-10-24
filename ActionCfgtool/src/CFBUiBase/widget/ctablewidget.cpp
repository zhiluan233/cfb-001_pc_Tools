

#include "ctablewidget.h"

#include <QTimerEvent>

CTableWidget::CTableWidget(QWidget *parent): QTableWidget(parent)
  ,m_nCurrentRowCount(0)
  ,m_nTimerIDUpdateRowChange(-1)
{

}

CTableWidget::~CTableWidget()
{

}


void CTableWidget::setAllCheckBoxs(QTableWidget *pTab, int nCheckBoxColPosition, Qt::CheckState state)
{
    if(pTab)
    {
        for(int i = 0; i < pTab->rowCount(); i++)
        {
            QCheckBox* pChkbox = dynamic_cast<QCheckBox*>(pTab->cellWidget(i, nCheckBoxColPosition));
            if(pChkbox)
            {
                pChkbox->setCheckState(state);
            }
        }
    }
}


bool CTableWidget::isAllCheckBoxsChecked(const QTableWidget * const pTab, const int nCheckBoxColPosition)
{
    if(!pTab || (pTab->rowCount() == 0))
    {
        return false;
    }

    bool bAllSelect = true;

    if(pTab)
    {
        for(int i = 0; i < pTab->rowCount(); i++)
        {
            QCheckBox* pCheckBox = dynamic_cast<QCheckBox*>(pTab->cellWidget(i, nCheckBoxColPosition));
            if(pCheckBox && !pCheckBox->isChecked())
            {
                bAllSelect = false;
                break;
            }
        }
    }

    return bAllSelect;
}

void CTableWidget::startRowChangeTimer(const int nMsec)
{
    if (m_nTimerIDUpdateRowChange > 0)
    {
        killTimer(m_nTimerIDUpdateRowChange);
    }

    m_nTimerIDUpdateRowChange = this->startTimer(nMsec);
}

void CTableWidget::timerEvent(QTimerEvent *event)
{
    if(event && (event->timerId() == m_nTimerIDUpdateRowChange))
    {
        if(m_nCurrentRowCount != this->rowCount())
        {
            m_nCurrentRowCount = this->rowCount();
            emit sigRowCountChanged(m_nCurrentRowCount);
        }
    }
}
