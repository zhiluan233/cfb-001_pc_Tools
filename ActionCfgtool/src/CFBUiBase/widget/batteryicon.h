

#ifndef CBATTERYICON_H
#define CBATTERYICON_H
#include "cfbuibase_global.h"
#include <QToolButton>
#include <QMap>


class CFBUIBASE_EXPORT CBatteryIcon : public QToolButton
{
public:
    explicit CBatteryIcon(QWidget *parent = NULL);
    ~CBatteryIcon();

public:
    enum emBatteryStatus
    {
        eBatteryDischarg = 0,  //00/否 01/是 02/没有电池
        eBatteryCharging = 1,
        eBatteryLineOff = 2,

        eBatteryUnknown
    };

public:
    static QString getBatteryUnknownIcon();
    static QString getBatteryLineOffIcon();
    static QString getBatteryChargingIcon(const int& nBatteryValue);
    static QString getBatteryDischargIcon(const int& nBatteryValue);

public slots:
    void setBattery(int eBatteryStatus, int nBatteryValue);

private:
    QString getIconPath(const int& eBatteryStatus,const int &nBatteryValue)const;
    void setBatteryIcon(const QString& iconPath);

private:

};

#endif // CBATTERYICON_H
