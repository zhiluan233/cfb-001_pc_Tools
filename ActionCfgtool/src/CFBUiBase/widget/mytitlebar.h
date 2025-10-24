

#ifndef MYTITLEBAR_H
#define MYTITLEBAR_H
#include "cfbuibase_global.h"
#include <QWidget>
#include <QLabel>
#include <QtUiPlugin/QDesignerExportWidget>

const QString MY_TITLE_BAR_OBJECT_NAME = "MyTitleBar";

namespace Ui {
class myTitleBar;
}

class CFBUIBASE_EXPORT  MyTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit MyTitleBar(QWidget *parent = 0);
    ~MyTitleBar();

public:
    enum emTitleSystemStyle
    {
        eTitleStyleMac = 0,
        eTitleStyleWin
    };


public:
    QLabel* getTitleLabel()const;
    void setTitleLabel(const QString& title);
    void setTitleStyle(emTitleSystemStyle style);

    //for control the button and title position
    void setLayoutMargins(int left, int top, int right, int bottom);


signals:
    void sigClose();


protected:
    virtual void showEvent(QShowEvent *);
        void paintEvent(QPaintEvent *);

protected:
virtual void resizeEvent(QResizeEvent *);

private:
    void resizeToParent();


private:
    Ui::myTitleBar *ui;
};

#endif // MYTITLEBAR_H
