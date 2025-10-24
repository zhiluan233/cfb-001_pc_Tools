

#ifndef CMENU_H
#define CMENU_H


#include "cfbuibase_global.h"
#include <QMenu>
#include <QColor>
#include <QFont>
#include <QImage>

const QString MENU_DATA_ALERT_KEY = "menuDataAlertKey";

class CFBUIBASE_EXPORT CMenu : public QMenu
{
    Q_OBJECT

public:
    explicit CMenu(const QString txt , QWidget* parent = 0);
    virtual ~CMenu();

public:
    //for set the alert text
    void setTextAlert(const QString &strAlert);
    //for set the alert color
    void setColorAlert(const QColor &colorAlert);
    //for set the  alert position
    void setAlertRightMargin(int nAlertRightMargin);
    //for set the  alert Font
    void setAlertFont(const QFont &font);
    //whether have alert mark flag
    bool isAlertMark()const;
    //for set the alert image
    void setAlertImage(const QString& strImgPath);

signals:
    void sigAlertMark(bool bAlertMark);


public slots:
    void setEnableAlert(bool bAlert);

protected:
    virtual  void paintEvent(QPaintEvent *);
    bool getAlertMarkAction(QList<QAction *>& actionList)const;
    static bool isActionAlertMark(QAction *pAction);

private:
    //repaint the item action for alert
    void repainItemAction(QMenu* pMenu, QAction* pAction);

private:
    QColor m_colorAlert;
    QFont m_font;
    QString m_strAlert;    
    QImage m_imagAlert;


    int m_nAlertRightMargin;
    bool m_bAlert;
};

#endif // CMENU_H
