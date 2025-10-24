

#ifndef QROUNDPROGRESSBAR_H
#define QROUNDPROGRESSBAR_H
#include "cfbuibase_global.h"
#include <QWidget>

class CFBUIBASE_EXPORT QRoundProgressBar : public QWidget
{
    Q_OBJECT
public:    
    explicit QRoundProgressBar(QWidget *parent = 0);

    static const int PositionLeft = 180;
    static const int PositionTop = 90;
    static const int PositionRight = 0;
    static const int PositionBottom = -90;

    
    double nullPosition() const { return m_nullPosition; }
    
    void setNullPosition(double position);

    
    enum BarStyle
    {
        /// Donut style (filled torus around the text)
        StyleDonut,
        /// Pie style (filled pie segment with the text in center)
        StylePie,
        /// Line style (thin round line around the text)
        StyleLine
    };
    
    void setBarStyle(BarStyle style);
    
    BarStyle barStyle() const { return m_barStyle; }

    
    void setOutlinePenWidth(double penWidth);
    
    double outlinePenWidth() const { return m_outlinePenWidth; }

    
    void setDataPenWidth(double penWidth);
    
    double dataPenWidth() const { return m_dataPenWidth; }

    
    void setDataColors(const QGradientStops& stopPoints);

    
    void setFormat(const QString& format);
    
    void resetFormat();
    
    QString	format() const { return m_format; }

    
    void setDecimals(int count);
    
    int decimals() const { return m_decimals; }

    
    double value() const { return m_value; }
    
    double minimum() const { return m_min; }
    
    double maximum() const { return m_max; }

public Q_SLOTS:
    
    void setRange(double min, double max);
    
    void setMinimum(double min);
    
    void setMaximum(double max);
    
    void setValue(double val);
    
    void setValue(int val);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void drawBackground(QPainter& p, const QRectF& baseRect);
    virtual void drawBase(QPainter& p, const QRectF& baseRect);
    virtual void drawValue(QPainter& p, const QRectF& baseRect, double value, double arcLength);
    virtual void calculateInnerRect(const QRectF& baseRect, double outerRadius, QRectF& innerRect, double& innerRadius);
    virtual void drawInnerBackground(QPainter& p, const QRectF& innerRect);
    virtual void drawText(QPainter& p, const QRectF& innerRect, double innerRadius, double value);
    virtual QString valueToText(double value) const;
    virtual void valueFormatChanged();

    virtual QSize minimumSizeHint() const { return QSize(32,32); }

    virtual bool hasHeightForWidth() const { return true; }
    virtual int heightForWidth(int w) const { return w; }

    void rebuildDataBrushIfNeeded();

    double m_min, m_max;
    double m_value;

    double m_nullPosition;
    BarStyle m_barStyle;
    double m_outlinePenWidth, m_dataPenWidth;

    QGradientStops m_gradientData;
    bool m_rebuildBrush;

    QString m_format;
    int m_decimals;

    static const int UF_VALUE = 1;
    static const int UF_PERCENT = 2;
    static const int UF_MAX = 4;
    int m_updateFlags;
};

#endif // QROUNDPROGRESSBAR_H
