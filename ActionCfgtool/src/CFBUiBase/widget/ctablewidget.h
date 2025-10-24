

#ifndef CTABLEWIDGET_H
#define CTABLEWIDGET_H


#include "cfbuibase_global.h"
#include <QTableWidget>
#include <QCheckBox>


class CFBUIBASE_EXPORT CTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit CTableWidget(QWidget *parent = NULL);
    ~CTableWidget();

public:
    
    static void setAllCheckBoxs(QTableWidget* pTab, int nCheckBoxColPosition, Qt::CheckState state);

    
    static bool isAllCheckBoxsChecked(const QTableWidget* const pTab, const int nCheckBoxColPosition);

    void startRowChangeTimer(const int nMsec);

signals:
    void sigRowCountChanged(int newCount);

protected:
    virtual void timerEvent(QTimerEvent* event);

private:
    int m_nCurrentRowCount;
    int m_nTimerIDUpdateRowChange;
};

#endif // CTABLEWIDGET_H
