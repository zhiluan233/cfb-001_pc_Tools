

#ifndef LEDSWIDGET_H
#define LEDSWIDGET_H

#include "motioneditor_global.h"
#include <QWidget>
#include <QPushButton>

const int LEDS_NUMBERS = 8;
enum emCheckBackgroundColor
{
    eBlackColor = 0,
    eRedColor,
    eGreenColor,
    eBlueColor,
    eYellowColor,
    ePinkColor,
    eIndigoColor,
    eWhiteColor
};

namespace Ui {
class CLedsWidget;
}
class MOTIONEDITOR_EXPORT CLedsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CLedsWidget(QWidget *parent = 0);
    ~CLedsWidget();

public:
    QPushButton *getBtn(const int& index)const;
    bool isChecked(const int& index)const;
    void setBackgroundColor(const int& btnId, const int& colorId);
    char getLedsSettings()const;
    void setLedsSettings(const int& nSettings);


signals:
    void sigButtonToggled(int index, bool bChecked);

protected slots:
    void onButtonPress();    
    
private:
    QString getBackgroundIconPath(const int& colorId);


private:
    Ui::CLedsWidget *ui;

};

#endif // LEDSWIDGET_H
