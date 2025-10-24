

#include <QTextBlock>

#include "ccommentwidget.h"
#include "ui_ccommentwidget.h"
#include "cfbwidgetconst.h"

CCommentWidget::CCommentWidget(QWidget *parent) :
    QWidget(parent),
    m_nMaxCommentCharNumber(CFBUiBaseConstants::COMMENT_MAX_CHARACTOR_NUMBER),
    ui(new Ui::CCommentWidget),
    m_nCommendId(0)
{
    ui->setupUi(this);

    ui->textEditComment->setPlaceholderText(tr("Please input the comment"));

    connect(ui->pushButtonComment, &QPushButton::clicked, this, &CCommentWidget::slotOnCommentClicked);
    connect(ui->textEditComment, &QTextEdit::textChanged, this, &CCommentWidget::slotOnCommentTextChanged);

    resetUI();
}

CCommentWidget::~CCommentWidget()
{
    delete ui;
}

void CCommentWidget::setMaxCommentCharNumber(int nNumber)
{
    if (nNumber <= 0)
    {
        return;
    }

    m_nMaxCommentCharNumber = nNumber;
}


void CCommentWidget::setReplyToWho(quint64 nCommentId, const QString &strWho)
{
    if (strWho.isEmpty())
    {
        return;
    }

    resetUI();

    m_nCommendId = nCommentId;
    m_strReplyHeader = QString("@%1:").arg(strWho);

    QFont previousFont;
    previousFont = ui->textEditComment->font(); // 记录历史信息

    QFont fontReplyToWho;
    fontReplyToWho.setPointSize(CFBUiBaseConstants::FONT_POINT_SIZE_10);
    ui->textEditComment->setFont(fontReplyToWho);
    ui->textEditComment->setTextColor(QColor(CFBUiBaseConstants::CFB_COLOR_DARK_GREY)); // #858585
    ui->textEditComment->insertPlainText(m_strReplyHeader);

    ui->textEditComment->setFont(previousFont); // 恢复原来的字体格式

    ui->textEditComment->setFocus();
}


void CCommentWidget::resetUI()
{
    m_strReplyHeader = "";
    m_nCommendId = 0;
    ui->textEditComment->clear();
    ui->textEditComment->setFocus();
    setCommentButtonState(false);

    ui->labelInputNumber->setText(QString("%1/%2").arg(0).arg(m_nMaxCommentCharNumber));
}


int CCommentWidget::getRealComment(QString &strRealComment)
{
     int nReplyId = 0;
    QString strCommentWithReplyToWho = ui->textEditComment->toPlainText().trimmed();
    if (strCommentWithReplyToWho.isEmpty())
    {
        return nReplyId;
    }

    QString strReplyHeader = m_strReplyHeader.trimmed(); // 评论头部，如@xx
    strRealComment = strCommentWithReplyToWho;
    if (strCommentWithReplyToWho.startsWith(strReplyHeader))
    {
        strRealComment = strCommentWithReplyToWho.right(strCommentWithReplyToWho.size() - strReplyHeader.size());
        nReplyId = m_nCommendId;
    }

    return nReplyId;
}

void CCommentWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        slotOnCommentClicked();
    }

    QWidget::keyPressEvent(event);
}


void CCommentWidget::setCommentButtonState(bool bEnable)
{
    ui->pushButtonComment->setEnabled(bEnable);
}


bool CCommentWidget::hasValidComment()
{
    QString strRealComment  = "";
    int nRepliedCommentId = getRealComment(strRealComment);
    if (strRealComment.isEmpty())
    {
        return false;
    }

    return true;
}


void CCommentWidget::slotOnCommentTextChanged()
{
    // 如果没有评论内容，则评论按钮不可用
    if (!hasValidComment())
    {
        setCommentButtonState(false);
    }
    else
    {
         setCommentButtonState(true);
    }

    QString strRealComment = "";
    int nRepliedCommentId = getRealComment(strRealComment);

    int nTextLength = strRealComment.count();
    if(nTextLength > m_nMaxCommentCharNumber)
    {
        int nPosition = ui->textEditComment->textCursor().position();
        QTextCursor textCursor = ui->textEditComment->textCursor();
        strRealComment.remove(nPosition - (nTextLength - m_nMaxCommentCharNumber), nTextLength - m_nMaxCommentCharNumber);
        ui->textEditComment->setText(m_strReplyHeader + strRealComment);
        textCursor.setPosition(nPosition - (nTextLength - m_nMaxCommentCharNumber));
        ui->textEditComment->setTextCursor(textCursor);
    }

    if (nTextLength > m_nMaxCommentCharNumber)
    {
        nTextLength = m_nMaxCommentCharNumber;
    }

    ui->labelInputNumber->setText(QString("%1/%2").arg(nTextLength).arg(m_nMaxCommentCharNumber));
}


void CCommentWidget::slotOnCommentClicked()
{
    if (!hasValidComment())
    {
        return;
    }

    QString strRealComment  = "";
    int nRepliedCommentId = getRealComment(strRealComment);
    if (strRealComment.isEmpty())
    {
        return;
    }

    setCommentButtonState(false);

    emit sigOnComment(nRepliedCommentId, strRealComment);
}
