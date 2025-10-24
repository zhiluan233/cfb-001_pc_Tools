

#ifndef CCOMMENTWIDGET_H
#define CCOMMENTWIDGET_H
#include "cfbuibase_global.h"
#include <QWidget>
#include <QKeyEvent>

namespace Ui {
class CCommentWidget;
}

class CFBUIBASE_EXPORT CCommentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CCommentWidget(QWidget *parent = 0);
    ~CCommentWidget();

    // 设置支持评论最大字符数目
    void setMaxCommentCharNumber(int nNumber);

    
    void setCommentButtonState(bool bEnable);

    
    void setReplyToWho(quint64 nCommentId, const QString &strWho);

    
    void resetUI();

    
    int getRealComment(QString &strRealComment);

protected:
    // ?????每调用起来
    virtual void keyPressEvent(QKeyEvent * event) override;

signals:
    
    void sigOnComment(quint64 nCommendId, const QString &strComment);

private slots:
    
    void slotOnCommentTextChanged();

    
    void slotOnCommentClicked();

private:
    
    bool hasValidComment();

private:
    int m_nMaxCommentCharNumber;  // 支持的评论最大字符数
    quint64 m_nCommendId;  // 被回复的评论Id, 默认值是0表示回复楼主

    QString m_strReplyHeader; // 评论的头部，如@xx

private:
    Ui::CCommentWidget *ui;
};

#endif // CCOMMENTWIDGET_H
