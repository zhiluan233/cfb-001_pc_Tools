
#ifndef CMESSAGEBOX_H
#define CMESSAGEBOX_H

#include "cfbuibase_global.h"
#include <QMessageBox>
#include <QVBoxLayout>

#include "mytitlebar.h"



class CFBUIBASE_EXPORT CMessageBox: public QWidget
{
    Q_OBJECT

public:
    explicit CMessageBox(QWidget *parent = 0);
    virtual ~CMessageBox();

public:
    MyTitleBar *getMyTitleBar()const;
    QMessageBox *getQMBox()const;

    void setMyTitleBarVisible(bool bVisible);


public:
    //asu, for warning box
    static QMessageBox::StandardButton warning(QWidget * parent,
                                               const QString & title,
                                               const QString & text,
                                               QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                               QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    //asu, for information box
    static QMessageBox::StandardButton information(QWidget * parent,
                                                   const QString & title,
                                                   const QString & text,
                                                   QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                   QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    //asu, for information box
    static QMessageBox::StandardButton question(QWidget * parent,
                                                const QString & title,
                                                const QString & text,
                                                QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    //asu, for information box
    static QMessageBox::StandardButton critical(QWidget * parent,
                                                const QString & title,
                                                const QString & text,
                                                QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);


private:
    //asu, for exec an message box
    static QMessageBox::StandardButton execMBox(QMessageBox::Icon icon,
                                                QWidget * parent,
                                                const QString & title,
                                                const QString & text,
                                                QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    //asu, set btn show text
    static void setBtnText(QAbstractButton *pBtn, QMessageBox::StandardButton);


private:
    MyTitleBar* m_pMyTitleBar;
    QMessageBox* m_pMessageBox;

    QVBoxLayout* m_pVLayout;
};

#endif // CMESSAGEBOX_H
