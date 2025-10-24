

#ifndef CFBMULTIPAGE_H
#define CFBMULTIPAGE_H
#include "cfbuibase_global.h"
#include <QWidget>
#include <QButtonGroup>

namespace Ui {
class CFBMultiPage;
}

class CFBUIBASE_EXPORT CFBMultiPage : public QWidget
{
    Q_OBJECT

public:
    explicit CFBMultiPage(QWidget *parent = 0);
    ~CFBMultiPage();

public:
    
    void UpdatePages(int nTotalPage);

    
    int getCurrentPage()const;

    
   void setCurrentPage(int nCurPage);

   
   int getTotalPage() const;

protected:
    
    void InitBtns();

signals:
    void sigCurrentPageChanged(int nCurPage);
    void sigPrevPageClicked();
    void sigNextPageClicked();

protected slots:
    
    void onBtnsButtonToggled(int id, bool bChecked);

    
    void onPrevPageClicked();

    
    void onNextPageClicked();

private:
     int m_nCurrentPage;//记录当前浏览的下载项页码
     int m_nTotalPage;
     int m_nPagebtns;//页码按钮个数

     QButtonGroup m_btnGroupForPages;//分页按钮组

private:
    Ui::CFBMultiPage *ui;
};

#endif // CFBMULTIPAGE_H
