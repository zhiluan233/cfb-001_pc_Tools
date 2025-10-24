
#include "OptionsMotionWidget.h"
#include "ui_UIOptionMotionWidget.h"
#include <QMessageBox>
#include <QComboBox>
#include <QMenu>
#include <QTimer>
#include <QMouseEvent>
#include <QClipboard>
#include <QMimeData>
#include <QDir>
#include "LayerUI/ActionBlockDataManager.h"
#include "dialogcommandbox.h"
#include "BatchModifyDialog.h"
#include "util.h"
#include "configs.h"

//UIMainLayerLayout

//int g_MotorCount = 16;
//QListWidgetItem setData Role 定义
//enum OptionItemRoles
//{
//    GroupID = Qt::UserRole + 1,
//    ActionBlockDataID
//};


//const int  GRID_COL_RUNTIME  = 16; //列表运行时间的列索引（0开始）
//const int  GRID_COL_ALLTIME  = GRID_COL_RUNTIME+1; //列表总时间的列索引

//const int GRID_COL_WIDTH = 50;
const int GRID_COL_WIDTH = 46;
const int GRID_COL_HEIGHT = 42;


bool CreateMotionLayOptionHandler(ILayerOptionHandler** ppHandler , emActionLayerType eActonType)
{
    COptionsMotionWidget * pProxy = new COptionsMotionWidget(NULL , eActonType);
    if(pProxy)
    {
        *ppHandler = pProxy;
        return true;
    }
    return false;
}

COptionsMotionWidget::COptionsMotionWidget(QWidget *parent , emActionLayerType eActionType) :
    QWidget(parent),
    ui(new Ui::COptionsMotionWidget),
    m_pGridTable(NULL),
    m_nCurrentGroupListSelectItem(-1),
    IsCompensatory(false),
    m_pEvent(NULL),
    m_pGroupListWidget(NULL),
    m_pMainLayer(NULL),
    m_nCurrentGroupID(-1),
    m_pGroupManager(NULL),
    m_nRunTimeCol(-1),
    m_nStopTimeCol(-1),
    m_nMoveCurvesCol(-1),
    m_eActionType(eActionType)
{
    ui->setupUi(this);

#ifdef Q_OS_WIN
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    m_pRobotView = NULL;
//    ui->leBeatValue->setEnabled(false);
    m_bIsTotalPlaying = false;
    m_nTotalTime = 0;
    m_strTotalTime.clear();

    connect(this, &COptionsMotionWidget::SigMotionPlayStop, this, &COptionsMotionWidget::OnMotionPlayStop);
    connect(this, &COptionsMotionWidget::SigSetControlState, this, &COptionsMotionWidget::OnSetControlState);

//    QRegExp regx("[1-9]{1}[0-9]{1,5}");
    QRegExp regx("[0-9]{1,5}");
    QValidator *validatorRunTime = new QRegExpValidator(regx, ui->lERunTime);
    ui->lERunTime->setValidator(validatorRunTime);

    QValidator *validatorStopTime = new QRegExpValidator(regx, ui->lEStopTime);
    ui->lEStopTime->setValidator(validatorStopTime);

    ui->m_ListActionGroup->setAttribute(Qt::WA_MacShowFocusRect, false);
//    ui->btnPause->hide();
//    connect(ui->btnPause, &QPushButton::clicked, this, &COptionsMotionWidget::on_btnUnitedSimulation_clicked);

//    ui->btnUnitedSimulation->setEnabled(false);
//    ui->btnStop->setEnabled(false);
    ui->pbInsert->setEnabled(false);
    ui->pbModify->setEnabled(false);
}



bool COptionsMotionWidget::Connect(QWidget *pParent, ILayerOptionHandler_Event *pEvent)
{
    setParent(pParent);
    QLayout* layout = pParent->layout();
    layout->addWidget(this);
    initWidget();
    m_pEvent = pEvent;
    return true;
}


void COptionsMotionWidget::initWidget()
{
    ui->lERunTime->setText(QString::number(20));
    ui->lEStopTime->setText(QString::number(0));
    m_pGroupListWidget = ui->m_ListActionGroup;
    m_pGridTable = ui->twActionList;
    initTableWidget();

    connect(m_pGridTable->verticalHeader(), &QHeaderView::sectionDoubleClicked,
            this, &COptionsMotionWidget::onSectionDoubleClicked);
    connect(m_pGridTable->itemDelegate(), &QAbstractItemDelegate::commitData,
            this, &COptionsMotionWidget::onItemCommitData);

    m_pGridTable->verticalHeader()->installEventFilter(this);

    if (ACTION_LAYER_MOTION != m_eActionType || ACTION_LAYER_HEAD != m_eActionType)
    {
        this->hide();
    }
}



void COptionsMotionWidget::onMotionMirror()
{
    CActionBlockGroupManager* pManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
    if (NULL == pManager)
    {
        return;
    }

    int nCurrentSelectRow = -1;

    QList<QTableWidgetSelectionRange> rangs = m_pGridTable->selectedRanges();
    int nRangeCount = rangs.count();

    for (int nRange = 0; nRange < nRangeCount; ++nRange)
    {
        int nTopRow = rangs.at(nRange).topRow();
        int nBottomRow = rangs.at(nRange).bottomRow();

        for (int j = nTopRow; j <= nBottomRow; ++j)
        {
            if (!IsFullRowSelect(j))
            {
                continue;
            }

            QTableWidgetItem* pItem = m_pGridTable->item(j, 0);
            if (NULL == pItem)
            {
                continue;
            }

            // 复制动作帧
            int nBlockDataID = pItem->data(ActionBlockDataID).toInt();
            CActionBlockData* pBlockData = pManager->FindBlockData(nBlockDataID);
            CActionBlockData* pActionBlockData = new CActionBlockData();

            char* pData = NULL;
            int nLen = 0;
            pBlockData->GetData(&pData, nLen);
            if (pData && nLen > 0)
            {
                pActionBlockData->SetData(pData, nLen);
                pActionBlockData->SetID(-1);

                delete[] pData;
            }

            // 镜像舵机数据
            char* pExtData = NULL;
            int   nExtLen = 0;
            pActionBlockData->GetExtBlockData(&pExtData, nExtLen);
            if (NULL == pExtData || nExtLen <= 0)
            {
                continue;
            }

            // 数据镜像，左右以120°对称
            int nCount = (nExtLen - sizeof(int)) / sizeof(MOTOR_DATA);
            for (int i = 0; i < nCount; ++i)
            {
                MOTOR_DATA* pMotor = (MOTOR_DATA*)(pExtData + sizeof(int) + i * sizeof(MOTOR_DATA));
                if (NULL == pMotor)
                {
                    continue;
                }

                int nMotorID = pMotor->nID;
                if (1 == nMotorID || 2 == nMotorID)             // 手部镜像
                {
                    MOTOR_DATA* pRightMotor = (MOTOR_DATA*)(pExtData + sizeof(int) + (i + 2) * sizeof(MOTOR_DATA));
                    if (NULL == pRightMotor)
                    {
                        continue;
                    }

                    int nLeftHand = pMotor->nAngle;
                    int nRightHand = pRightMotor->nAngle;

                    pMotor->nAngle = 240 - nRightHand;
                    pRightMotor->nAngle = 240 - nLeftHand;
                }
                else if (5 == nMotorID || 6 == nMotorID)        // 腿部镜像
                {
                    MOTOR_DATA* pRightMotor = (MOTOR_DATA*)(pExtData + sizeof(int) + (i + 3) * sizeof(MOTOR_DATA));
                    if (NULL == pRightMotor)
                    {
                        continue;
                    }

                    int nLeftLeg = pMotor->nAngle;
                    int nRightLeg = pRightMotor->nAngle;

                    pMotor->nAngle = 240 - nRightLeg;
                    pRightMotor->nAngle = 240 - nLeftLeg;
                }
                else
                {
                    continue;
                }
            } // end of for 数据镜像

            pActionBlockData->SetExtBlockData(nExtLen, pExtData);

            int nCurrentID = GetCurrenGroupManagerID();

            if (j == nBottomRow)
            {
                m_pEvent->ILayerOptionHandler_OnAddActionBlock(nCurrentID, pActionBlockData, "", true);
            }
            else
            {
                m_pEvent->ILayerOptionHandler_OnAddActionBlock(nCurrentID, pActionBlockData);
            }

            if (nCurrentID != -1)
            {
                int nID = pActionBlockData->GetID();
                SetShowID(&nID, 1);

                QString strTmpDescription = QString::fromStdWString(pActionBlockData->GetDescription());

                nCurrentSelectRow = AddToTableWidget(pExtData, nExtLen, strTmpDescription, pActionBlockData->GetRunTime(), pActionBlockData->GetStopTime(), pActionBlockData->GetID());
                if (pExtData)
                {
                    delete[] pExtData;
                }
            }
        }
    }

    ShowSelectRow(nCurrentSelectRow);
}



void COptionsMotionWidget::onMotionCopy()
{
    if (m_nCurrentGroupID == -1)
        return;
    if (m_pGroupManager == NULL)
        return;
    if(m_pGridTable == NULL)
    {
        return;
    }

    CActionBlockGroupManager* pManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
    if (pManager == NULL)
        return;

    QList<QTableWidgetSelectionRange>ranges= m_pGridTable->selectedRanges();
    int nSelectRowCount = 0;
    int nRangeCount=ranges.count();
    for(int ir = 0;ir < nRangeCount;ir++)
    {
        nSelectRowCount += ranges.at(ir).rowCount();
    }

    vector<char> RawData;

    // 保存选择的行数
    AddVectorData(&RawData, (char*)&nSelectRowCount, sizeof(nSelectRowCount));

    QTableWidgetItem* pItem = NULL;

    for (int i=0; i < nRangeCount; i++)
    {
        int nTopRow = ranges.at(i).topRow();
        int nBottomRow = ranges.at(i).bottomRow();
        for(int j = nTopRow ;j <= nBottomRow;j++)
        {
            bool bIsFullRowSelect = IsFullRowSelect(j);
            if(bIsFullRowSelect)
            {

                pItem = m_pGridTable->item(j,0);
                int nBlockDataID = pItem->data(ActionBlockDataID).toInt();
                CActionBlockData* pBlockData = pManager->FindBlockData(nBlockDataID);
                if (pBlockData == NULL)
                    continue;

                char* pData = NULL;
                int nLen = 0;
                pBlockData->GetData(&pData, nLen);
                if (pData)
                {
                    // 先保存长度
                    AddVectorData(&RawData, (char*)&nLen, sizeof(nLen));
                    // 保存数据
                    AddVectorData(&RawData, pData, nLen);

                    delete [] pData;
                }
            }

        }

    }

    int nTotalLen = RawData.size();
    char* pTotalData = new char[nTotalLen];
    CopyVertorToPointer(RawData, pTotalData);


    QClipboard *clipboard = QApplication::clipboard();
    clipboard->clear();
    QByteArray bArray(pTotalData,nTotalLen);

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("CopyData", bArray);

    clipboard->setMimeData(mimeData);

    //Delete by jianjie 2017/7/15 剪切板数据不能删除，否则复制无效
//     if(mimeData)
//     {
//         delete mimeData;
//         mimeData = NULL;
//     }

    if (pTotalData && nTotalLen)
    {
        delete [] pTotalData;
    }
}

void COptionsMotionWidget::onMotionPaste()
{

    const QClipboard *clipboard = QApplication::clipboard();
    const  QMimeData* mimeData = clipboard->mimeData();
    if(mimeData == NULL)
    {
        qDebug()<<tr("can't find mimeData point")<<endl;
        return;
    }
    QByteArray bArray = mimeData->data("CopyData");
    char * buffer = bArray.data();
    int nTotalLen = bArray.length();
    if(nTotalLen <= 0)
    {
        return;
    }

    int nTotalRow = 0;
    CFB_MEMCPY_INC(nTotalRow, buffer);

    int nCurrentSelectRow = -1;
    for (int i=0; i<nTotalRow; i++)
    {
        int nLen =0;
        CFB_MEMCPY_INC(nLen, buffer);

        char* pData = new char[nLen];
        memcpy(pData, buffer, nLen);
        buffer += nLen;

        CActionBlockData* pActionBlockData = new CActionBlockData();
        pActionBlockData->SetData(pData, nLen);
        pActionBlockData->SetID(-1);

        if (pData)
        {
            delete [] pData;
        }

        int nCurrentID = GetCurrenGroupManagerID();
        if(i == nTotalRow - 1)
        {
            m_pEvent->ILayerOptionHandler_OnAddActionBlock(nCurrentID, pActionBlockData ,"", true);
        }
        else
        {
            m_pEvent->ILayerOptionHandler_OnAddActionBlock(nCurrentID, pActionBlockData);
        }

        if (nCurrentID != -1)
        {
            int nID = pActionBlockData->GetID();
            SetShowID(&nID, 1);

            char* pExtDate = NULL;
            int nExtLen = 0;
            pActionBlockData->GetExtBlockData(&pExtDate, nExtLen);

            QString strTmpDescription = QString::fromStdWString(pActionBlockData->GetDescription());
            //QString strTmpDescription = QString::fromUtf16(pActionBlockData->GetDescription());

            nCurrentSelectRow = AddToTableWidget(pExtDate, nExtLen, strTmpDescription, pActionBlockData->GetRunTime(), pActionBlockData->GetStopTime(), pActionBlockData->GetID());
            if (pExtDate)
            {
                delete [] pExtDate;
            }
        }
    }
    ShowSelectRow(nCurrentSelectRow);
}


void COptionsMotionWidget::onMotionInsert()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const  QMimeData* mimeData = clipboard->mimeData();
    if(mimeData == NULL)
    {
        qDebug()<<tr("can't find mimeData point")<<endl;
        return;
    }
    QByteArray bArray = mimeData->data("CopyData");
    char * buffer = bArray.data();
    int nTotalLen = bArray.length();
    if(nTotalLen <= 0)
    {
        return;
    }

    if (m_pGroupManager == NULL ||
            m_pGridTable == NULL)
        return;
    int nSelectBlockID = getFirstSelectRowID();
    CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);

    int nTotalRow = 0;
    CFB_MEMCPY_INC(nTotalRow, buffer);

    if(pGroupManager == NULL)
        return;

    for (int i = 0; i < nTotalRow; i++)
    {

        int nLen = 0;
        CFB_MEMCPY_INC(nLen, buffer);
        char* pData = new char[nLen];
        memcpy(pData, buffer, nLen);
        buffer += nLen;
        CActionBlockData* pActionBlockData = new CActionBlockData();
        pActionBlockData->SetData(pData, nLen);
        pActionBlockData->SetID(-1);
        if(pData)
        {
            delete [] pData;
        }
        pGroupManager->InsertBeforeId(nSelectBlockID, pActionBlockData);
        int nCurrentShowID = pActionBlockData->GetID();
        SetShowID(&nCurrentShowID, 1);

        int nCurrentID = m_nCurrentGroupID;
        m_nCurrentGroupID = -1;
        if(i == nTotalRow - 1)
        {
            g_IsUpdateUIByData = true;
            m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, pActionBlockData->GetStopTime(), true);
            g_IsUpdateUIByData = false;
        }
        else
        {
            g_IsUpdateUIByData = true;
            m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, pActionBlockData->GetStopTime(), false);
            g_IsUpdateUIByData = false;
        }
    }
    UpdateUIByData();
}

//int  COptionsMotionWidget::getSelectRowCount()
//{

//    int nSelectCount = 0;
//    for (int i=0; i<m_pGridTable->rowCount(); i++)
//    {
//        if (IsFullRowSelect(i))
//            nSelectCount++;
//    }

//    return nSelectCount;

//}

int COptionsMotionWidget::getFirstSelectRowID()
{
    int nSelectID = -1;
    QList<QTableWidgetSelectionRange> ranges = m_pGridTable->selectedRanges();
    int nRow = ranges.first().topRow();
    QTableWidgetItem* pItem = m_pGridTable->item(nRow, 0);
    nSelectID= pItem->data(ActionBlockDataID).toInt();
    return nSelectID;
}

MOTOR_DATA *COptionsMotionWidget::GetMotorData(char *pData, int nLen, int nID)
{
    MOTOR_DATA* p = (MOTOR_DATA*)(pData + 4);
    return &p[nID];
}

void COptionsMotionWidget::SetMotorAngle(char *pData, int nLen, int nID, int nAngle)
{
    //int nCount = (nLen-4)/sizeof(MOTOR_DATA);
    MOTOR_DATA* p = (MOTOR_DATA*)(pData+4);
    p[nID].nAngle = nAngle;
}

void COptionsMotionWidget::ModleView(int nActionBlockID , int nOffset)
{
    if(NULL == m_pGroupManager || NULL == m_pRobotView)
        return;

    CActionBlockGroupManager* pGroupData = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
    if (NULL == pGroupData)
        return;

    CActionBlockData* pActionBlockData = pGroupData->FindBlockData(nActionBlockID);
    if (NULL == pActionBlockData)
    {
        return;
    }
    char* pData = NULL;
    int nLen = 0;
    pActionBlockData->GetExtBlockData(&pData, nLen);
    if (NULL == pData || nLen <= 0)
    {
        return;
    }
    int nBlockTime = pActionBlockData->GetRunTime();
    int nScaleTime = 100;
    if (m_pEvent)
    {
        nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();
    }
    int nRunTime = nBlockTime * nScaleTime;

    // 注意存在停止时间
    if (nOffset > nBlockTime)
    {
        nOffset = nBlockTime;
    }

    // 鼠标单击时间轴，动作定位到具体时刻
    if (nOffset > 0)
    {
        int nMotroCount = (nLen - sizeof(int)) / sizeof(MOTOR_DATA);
        MOTOR_DATA* pMotorData = (MOTOR_DATA*)(pData + sizeof(int));

        char* pPreData = NULL;
        int nPreLen = 0;
        CActionBlockData* pPreBlockData = NULL;
        MOTOR_DATA* pPreMotorData = NULL;

        // 存在前一帧
        int nBlockIndex = pGroupData->GetIndexByID(nActionBlockID);
        if (nBlockIndex > 0)
        {
            pPreBlockData = pGroupData->GetAt(nBlockIndex - 1);
            if (NULL == pPreBlockData)
            {
                return;
            }
            pPreBlockData->GetExtBlockData(&pPreData, nPreLen);
            pPreMotorData = (MOTOR_DATA*)(pPreData + sizeof(int));

            for (int i = 0; i < nMotroCount; ++i)
            {
                pMotorData[i].nAngle = pPreMotorData[i].nAngle + (pMotorData[i].nAngle - pPreMotorData[i].nAngle) * nOffset / nBlockTime;
            }
        }
        else
        {
            int nGroupIndex = m_pGroupManager->GetIndexByID(m_nCurrentGroupID);
            if (nGroupIndex > 0)    // 不存在前一帧，但存在前一组
            {
                CActionBlockGroupManager* pPreGroupData = m_pGroupManager->GetAt(nGroupIndex - 1);
                pPreBlockData = pPreGroupData->GetAt(pPreGroupData->GetActionCount() - 1);
                if (NULL == pPreBlockData)
                {
                    return;
                }
                pPreBlockData->GetExtBlockData(&pPreData, nPreLen);
                pPreMotorData = (MOTOR_DATA*)(pPreData + sizeof(int));

                for (int i = 0; i < nMotroCount; ++i)
                {
                    pMotorData[i].nAngle = pPreMotorData[i].nAngle + (pMotorData[i].nAngle - pPreMotorData[i].nAngle) * nOffset / nBlockTime;
                }
            }
            else        // 前一帧、前一组均不存在
            {
                for (int i = 0; i < nMotroCount; ++i)
                {
                    pMotorData[i].nAngle = 120 + (pMotorData[i].nAngle - 120) * nOffset / nBlockTime;
                }
            }
        }

        nRunTime = 300;
        if (pPreData)
        {
            delete[] pPreData;
        }
    } 

    //m_pRobotView->SetMotorDatasManual(pData, nLen, nRunTime);
    m_pRobotView->SetMotorDatas(pData, nLen, nRunTime);
    m_pRobotView->SetMotorDatasSmoothly(pData, nLen, nRunTime);

    delete[] pData;

}


void COptionsMotionWidget::GetRunTimeAndStopTime(int &nRunTime, int &nStopTime)
{
    QString strRunTime = ui->lERunTime->text();
    nRunTime = strRunTime.toInt();//运行时间

    QString strStopTime = ui->lEStopTime->text();
    nStopTime = strStopTime.toInt();//停止时间
}


//int COptionsMotionWidget::GetCurMotionNodeIDFromFile()
//{
//   QString strConfigFile = CConfigs::getLocalUserConfigPath("config.ini");
//    QSettings settings(strConfigFile, QSettings::IniFormat);
//   settings.beginGroup("Config");
//    m_nCurMotionNodeID = settings.value("CurrentActionId", -1).toInt();
//}


void COptionsMotionWidget::ReleaseConnection()
{
    disconnect();
    m_ShowId.clear();
    if(m_pGridTable != NULL)
    {
        m_pGridTable->clearContents();
        m_pGridTable->setRowCount(0);
        m_pGridTable = NULL;
    }

    IsCompensatory = false;
    m_nCurrentGroupListSelectItem = -1;

    m_nCurrentGroupID = -1;
    //m_bLbtnDownDebug = false;
    m_ShowId.clear();

    deleteLater();
}


COptionsMotionWidget::~COptionsMotionWidget()
{
    delete ui;
    if(m_pGridTable)
    {
        delete m_pGridTable;
        m_pGridTable = NULL;
    }
}


bool COptionsMotionWidget::eventFilter(QObject *obj, QEvent *event)
{
    int nType = event->type();
    if (nType == QEvent::ContextMenu)
    {
        CreateGridListPopMenuActions();
    }
    return QWidget::eventFilter(obj, event);
}


void COptionsMotionWidget::keyPressEvent(QKeyEvent *event)
{
    // Ctrl+C 复制
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_C)
    {
        QString strCopyData = "";
        QList<QTableWidgetItem*> items = m_pGridTable->selectedItems();
        for (int i = 0; i < items.count(); ++i)
        {
            QTableWidgetItem* pItem = items.at(i);
            QString str = pItem->text();
            strCopyData += str + "\n";
        }

        // Add by jianjie 2017/12/04 剪贴板无法写入数据，暂时用文件过度数据，有更好方法当修改此处
        QString strCopyDataPath = CConfigs::getUserDirPath(USERDIR_TEMP) + QDir::separator() + "temp.txt";
        QFile file(strCopyDataPath);
        QByteArray byteData = strCopyData.toLatin1();

        if (file.open(QIODevice::WriteOnly))
        {
            file.write(byteData);
        }
        file.close();

        // 剪贴板不能在子线程或Gui线程中使用，此处不能使用剪贴板
        //QClipboard *clipboard = QApplication::clipboard();
        //clipboard->clear();
        //clipboard->setText(m_strCopyData);
    }
    // Ctrl + V 粘贴
    else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_V)
    {
        QString strCopyDataPath = CConfigs::getUserDirPath(USERDIR_TEMP) + QDir::separator() + "temp.txt";
        QFile file(strCopyDataPath);

        QString strPasteData = "";
        if (file.open(QIODevice::ReadOnly))
        {
            QByteArray byteData = file.readAll();
            strPasteData = byteData;
        }
        file.close();

        QList<QTableWidgetItem*> items = m_pGridTable->selectedItems();
        for (int i = 0; i < items.count(); ++i)
        {
            if (strPasteData.isEmpty())
            {
                break;
            }

            QTableWidgetItem* pItem = items.at(i);
            int nRow    = pItem->row();
            int nColumn = pItem->column();

            int nIndex = strPasteData.indexOf("\n");
            QString strItem = strPasteData.left(nIndex);
            strPasteData = strPasteData.mid(nIndex + 1);

            QTableWidgetItem* pItem0 = m_pGridTable->item(nRow, 0);
            int nActionBlockID = pItem0->data(ActionBlockDataID).toInt();
            CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
            if (!pGroupManager)
            {
                break;
            }

            CActionBlockData* pBlockData = pGroupManager->FindBlockData(nActionBlockID);
            if (!pBlockData)
            {
                break;
            }

            // 舵机数据
            if (nColumn < m_nRunTimeCol)
            {
                QString strAngle = strItem;
                bool    bOk = false;
                int     nAngle = strAngle.toInt(&bOk);

                // 数据不合格，恢复数据
                char* pData = NULL;
                int nLen = 0;
                pBlockData->GetExtBlockData(&pData, nLen);

                if (pData && nLen)
                {
                    if (nAngle > GRID_MAX_ANGLEVALUE || bOk == false)
                    {
                        MOTOR_DATA* pMoto = GetMotorData(pData, nLen, nColumn);
                        if (pMoto)
                        {
                            strAngle.sprintf("%d", pMoto->nAngle);
                        }
                    }
                    else
                    {
                        // Modified Flag
                        SetMotorAngle(pData, nLen, nColumn, nAngle);
                        if (m_pEvent)
                        {
                            m_pEvent->ILayerOptionHandler_SetModifiedState(true);
                        }
                        pBlockData->SetExtBlockData(nLen, pData);
                    }
                    delete[] pData;
                }

                pItem->setText(strAngle);
            }
            else if (nColumn == m_nRunTimeCol
                || nColumn == m_nStopTimeCol)//修改了运动时间或停止时间
            {
                int nTimeInc = 0;
                bool bIsRunTimeCol = (nColumn == m_nRunTimeCol);

                QString sTempTime = strItem;
                bool    bOk = false;
                int     nRealTime = sTempTime.toInt(&bOk);

                //无效数据
                if (!bOk || nRealTime > GRID_MAX_TIMEVALUE || nRealTime < GRID_MIN_TIMEVALUE)
                {
                    int nTempTime = (bIsRunTimeCol ? pBlockData->GetRunTime() : pBlockData->GetStopTime());
                    sTempTime.sprintf("%d", nTempTime);
                }
                else
                {
                    if (bIsRunTimeCol)
                    {
                        nTimeInc = nRealTime - pBlockData->GetRunTime();
                        pBlockData->SetRunTime(nRealTime);
                    }
                    else
                    {
                        nTimeInc = nRealTime - pBlockData->GetStopTime();
                        pBlockData->SetStopTime(nRealTime);
                    }
                    pItem->setText(sTempTime);
                }

                pItem->setText(sTempTime);

                // 分组总时间改变通知
                if (m_pEvent)
                {
                    m_pEvent->ILayerOptionHandler_OnActionBlockDataChange(m_nCurrentGroupID, nActionBlockID, nTimeInc);
                }
            }

        }
    }
}

//void COptionsMotionWidget::mousePressEvent(QMouseEvent *event)
//{
//    if(event->button() == Qt::RightButton)
//    {
//        CreateGridListPopMenuActions();
//    }
//}

//bool COptionsMotionWidget::Connect(QWidget *pWnd)
//{

//}

//void COptionsMotionWidget::Disconnect()
//{

//}

QWidget *COptionsMotionWidget::GetWidget()
{
    return this;
}


void COptionsMotionWidget::SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID, int nOffset)
{
    //    bool IsUpdata = true;
    //    if (m_nCurrentGroupID == nCurrentGroupID)
    //    {
    //        IsUpdata = false;
    //    }
    if (nCurrentGroupID == 0)
    {
        m_nCurrentGroupID = nCurrentGroupID;
        m_pGroupManager = pManager;
    }

    m_nCurrentGroupID = nCurrentGroupID;
    m_pGroupManager = pManager;

    if (nCurrentGroupID == -1)
    {
//        ui->leBeatValue->setEnabled(false);
        m_nCurrentGroupListSelectItem = -1;
        m_ShowId.clear();
    }

    if (g_IsModifyGroup || g_IsUpdateUIByData)
    {
        UpdateUIByData();
    }

    if(m_pGroupListWidget == NULL)
    {
        return;
    }
    else
    {
        QListWidgetItem * pItem = NULL;
        for (int i=0; i<m_pGroupListWidget->count(); i++)
        {
            pItem= m_pGroupListWidget->item(i);
            if(pItem == NULL)
            {
                qDebug()<<"can't find pItem point"<<endl;
                break;
            }
            QVariant value = pItem->data(GroupID);
            int nValue = value.toInt();
            if (nValue == nCurrentGroupID)
            {
                m_nCurrentGroupListSelectItem = i;
                break;
            }
        }
    }

    if (g_IsModifyGroup || g_IsUpdateUIByData)
    {
        QListWidgetItem * item =m_pGroupListWidget->item(m_nCurrentGroupListSelectItem);
        SetCurrentSelectItem(item,m_nCurrentGroupListSelectItem);
        g_IsUpdateUIByData = false;
        g_IsModifyGroup  = false;
    }

    if (nCurrentActionID != -1 && m_nCurrentGroupListSelectItem != -1)
    {
        int nActionID = 0;
        if (pManager)
        {
            CActionBlockData *pData = pManager->GetAt(m_nCurrentGroupListSelectItem)->GetAt(nCurrentActionID);
            if (pData)
            {
                nActionID = pData->GetID();
                SetShowID(&nActionID,1);
                ShowSelectRow(nCurrentActionID);

                ModleView(nActionID, nOffset);   //更新3D模型
            }
        }
    }
}

void COptionsMotionWidget::Set3DRobotViewInterface(ICFBRobot3DView *pRobotView)
{
    m_pRobotView = pRobotView;
}

void COptionsMotionWidget::SetControlState(bool controlState, int nLayerType)
{
    emit SigSetControlState(controlState, nLayerType);
}

void COptionsMotionWidget::InsertGroupByMenu(int nCol)
{
    if(m_pEvent && m_pRobotView)
    {
        char* pData = NULL;
        int nLen = 0;
        m_pRobotView->GetMotorDatas(&pData, nLen, m_eActionType);
        if (pData && nLen)
        {
//            QString strRunTime = ui->lERunTime->text();
//            int nRunTime = strRunTime.toInt();//运行时间

//            QString strStopTime = ui->lEStopTime->text();
//            int nStopTime = strStopTime.toInt();//停止时间
            int nRunTime =0;
            int nStopTime = 0;
            GetRunTimeAndStopTime(nRunTime, nStopTime);

//            int nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();
            CActionBlockData* pActionBlockData = new CActionBlockData();
            pActionBlockData->SetID(-1);

//            pActionBlockData->SetDescription(L"Action");
            std::wstring wsDescription = L"Action";
            pActionBlockData->SetDescription((WCHAR *)wsDescription.c_str());
            pActionBlockData->SetRunTime(nRunTime);
            pActionBlockData->SetStopTime(nStopTime);
            pActionBlockData->SetExtBlockData(nLen, pData);
            m_pEvent->ILayerOptionHandler_InsertGroupByMenu(pActionBlockData, nCol);
        }
    }
}

void COptionsMotionWidget::MotionPlayStop()
{
    emit SigMotionPlayStop();
}

//void COptionsMotionWidget::FinishPlayingMusic()
//{
//    //ui->btnUnitedSimulation->setText(tr("United Simulation"));
//    ui->btnPause->hide();
//    ui->btnUnitedSimulation->show();
//    m_bIsTotalPlaying = false;
//    ui->musicSlider->setSliderPosition(0);
//}

//void COptionsMotionWidget::MusicLoaded(const QString& title, int time)
//{
//    ui->labelMusicName->setText(title);
//    m_strTotalTime = QString().sprintf("%02d:%02d:%02d",
//                                       (int)(time) / 3600,
//                                       ( (int)(time) % 3600 ) / 60,
//                                       (int)(time) % 60);
//    ui->labelMusicTime->setText(m_strTotalTime);
//    m_nTotalTime = time;
//    ui->musicSlider->setRange(0, time);
//    ui->musicSlider->setSliderPosition(0);
//}

//void COptionsMotionWidget::UpdateMusicPosition(quint64 position)
//{
//    int timeSec = position / 1000;//转为秒
//    QString strPosition = QString().sprintf("%02d:%02d:%02d",
//                                            (int)(timeSec) / 3600,
//                                            ( (int)(timeSec) % 3600 ) / 60,
//                                            (int)(timeSec) % 60);
//    ui->labelMusicTime->setText(strPosition + '/' + m_strTotalTime);
//    ui->musicSlider->setSliderPosition(timeSec);
//}


void COptionsMotionWidget::on_pbAdd_clicked()
{
    //  AddToTableWidget();
    //AddSampleListToTableWidget();
    //不能直接使用数字10作为时间刻度
    if (!m_pEvent)
    {
        return;
    }
//    int nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue() ;//时间单位（ms）
    if (m_pEvent && m_pRobotView)
    {
        char* pData = NULL;
        int nLen = 0;

        m_pRobotView->GetMotorDatas(&pData, nLen, m_eActionType);
        if (pData && nLen)
        {
//            QString strRunTime = ui->lERunTime->text();
//            int nRunTime = strRunTime.toInt();//运行时间

//            QString strStopTime = ui->lEStopTime->text();
//            int nStopTime = strStopTime.toInt();//停止时间
            int nRunTime =0;
            int nStopTime = 0;
            GetRunTimeAndStopTime(nRunTime, nStopTime);

            CActionBlockData* pActionBlockData = new CActionBlockData();
            pActionBlockData->SetID(-1);
            std::wstring wsDescription = L"Action";
            pActionBlockData->SetDescription((WCHAR *)wsDescription.c_str());
            pActionBlockData->SetRunTime(nRunTime);
            pActionBlockData->SetStopTime(nStopTime);//设置停止时间
            pActionBlockData->SetExtBlockData(nLen, pData);
            int nCurrentGroupID = GetCurrenGroupManagerID();

            g_IsUpdateUIByData = true;   //这里默认需要修改
            g_IsModifyGroup = true;      //这里默认需要修改
            m_pEvent->ILayerOptionHandler_OnAddActionBlock(nCurrentGroupID, pActionBlockData, "", true);
            g_IsUpdateUIByData = false;
            //                       float fAllTime = nRunTime*(nID+1);
            if (nCurrentGroupID != -1)
            {
                int nID = pActionBlockData->GetID();

                SetShowID(&nID, 1);
                //QString strTmpDescription = WCHARToQString(pActionBlockData->GetDescription(), MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
                QString strTmpDescription = QString::fromStdWString(pActionBlockData->GetDescription());
                //QString strTmpDescription = QString::fromUtf16(pActionBlockData->GetDescription());
                int nRow = AddToTableWidget(pData, nLen, strTmpDescription, pActionBlockData->GetRunTime(), pActionBlockData->GetStopTime(), nID);
                ShowSelectRow(nRow);
                // ShowSelectActionBlock();
            }
            else
            {
                int nID = pActionBlockData->GetID();
                SetShowID(&nID, 1);
                // ShowSelectActionBlock();
            }
            delete [] pData;
        }
    }
}


void COptionsMotionWidget::on_pbInsert_clicked()
{
    if (m_pGroupManager == NULL ||
            m_pGridTable == NULL)
        return;
    CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
    if(pGroupManager == NULL)
        return;
    int nCurRow = m_pGridTable->currentRow();
    //fix bug 148
    //int nCount = m_pGridTable->rowCount();
    //for (int i = 0; i < nCount; i++)
    //    {
    //        bool bSelectFullLine = true;
    //        bSelectFullLine = IsFullRowSelect(nCurRow);

    //        if (bSelectFullLine)
    //        {
    QTableWidgetItem* pItem = m_pGridTable->item(nCurRow, 0);
    int nSelectBlockID = pItem->data(ActionBlockDataID).toInt();
    if (m_pEvent && m_pRobotView)
    {
        char* pData = NULL;
        int nLen = 0;
        m_pRobotView->GetMotorDatas(&pData, nLen, m_eActionType);
        if (pData && nLen)
        {
//            int nRunTime = 0;
//            if (ui->leBeatValue->isEnabled())
//            {
//                QString tempValue = ui->leBeatValue->text();
//                nRunTime = tempValue.toInt();
//                if (nRunTime < 10)
//                {
//                    nRunTime = 10;
//                }
//            }
//            int nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();//时间单位

//            QString strRunTime = ui->lERunTime->text();
//            int nRunTime = strRunTime.toInt();//运行时间

//            QString strStopTime = ui->lEStopTime->text();
//            int nStopTime = strStopTime.toInt();//停止时间
            int nRunTime =0;
            int nStopTime = 0;
            GetRunTimeAndStopTime(nRunTime, nStopTime);

            CActionBlockData* pActionBlockData = new CActionBlockData();
            pActionBlockData->SetID(pGroupManager->GetUnUserID());
            std::wstring wsDescription = L"Action";
            pActionBlockData->SetDescription((WCHAR*)wsDescription.c_str());
            pActionBlockData->SetRunTime(nRunTime);
            pActionBlockData->SetStopTime(nStopTime);
            pActionBlockData->SetExtBlockData(nLen, pData);

            pGroupManager->InsertBeforeId(nSelectBlockID, pActionBlockData);

            int nCurrentShowID = pActionBlockData->GetID();
            SetShowID(&nCurrentShowID, 1);
            int nCurrentID = m_nCurrentGroupID;
            m_nCurrentGroupID = -1;
            g_IsUpdateUIByData = true;
            m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, pActionBlockData->GetRunTime() + pActionBlockData->GetStopTime(), true);
            g_IsUpdateUIByData = false;
            delete[] pData;
        }
    }
    ShowSelectRow(nCurRow);
}


int COptionsMotionWidget::GetCurrenGroupManagerID()
{
    if (m_nCurrentGroupListSelectItem == -1)
        return -1;

    if (m_pGroupManager == NULL)
        return -1;

    CActionBlockGroupManager* pGroupManager = m_pGroupManager->GetAt(m_nCurrentGroupListSelectItem);
    if (pGroupManager)
    {
        return pGroupManager->GetID();
    }

    return -1;
}

void COptionsMotionWidget::SetShowID(int *pID, int nCount)
{
    m_ShowId.clear();

    for(int i=0; i<nCount; i++)
    {
        m_ShowId.push_back(pID[i]);
    }
}


int COptionsMotionWidget::AddToTableWidget(char *pData, int nLen, QString BlockType, int nRunTime, int nStopTime, int ActionBlockID)
{

    int nLens       = 0;
    int nCurveModel = 0;

    memcpy(&nLens, pData,sizeof(nLens));
    int nCount = (nLens - 4)/sizeof(MOTOR_DATA);
    MOTOR_DATA* pMotor = (MOTOR_DATA*)(pData + 4);
    memcpy(&nCurveModel , pData + 4 + nCount * sizeof(MOTOR_DATA) , sizeof(nCurveModel));

    if(m_pGridTable == NULL)
    {
        qDebug()<<"can't find m_pGridTable"<<endl;
        return -1;
    }

    int nRow=m_pGridTable->rowCount();
    m_pGridTable->insertRow(nRow);

    QString strtext = "";
    for(int i=0;i<nCount;i++)
    {
        strtext = QString::number(pMotor[i].nAngle);
        QTableWidgetItem* pItem = new QTableWidgetItem(strtext);
        pItem->setTextAlignment(Qt::AlignCenter);
        m_pGridTable->setItem(nRow,i,pItem);
    }
//    int nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();
    QString strRuntime = QString::number(nRunTime);
    QTableWidgetItem*  pRuntimeItem = new QTableWidgetItem(strRuntime);
    pRuntimeItem->setTextAlignment(Qt::AlignCenter);
    m_pGridTable->setItem(nRow,nCount,pRuntimeItem);

    QString strStopTime = QString::number(nStopTime);
    QTableWidgetItem* pStopTimeItem = new QTableWidgetItem(strStopTime);
    pStopTimeItem->setTextAlignment(Qt::AlignCenter);
    m_pGridTable->setItem(nRow,nCount+1,pStopTimeItem);

    QComboBox* pComboBox = new QComboBox;
    pComboBox->addItem(tr("linear"));
    pComboBox->addItem(tr("curve"));
    pComboBox->setCurrentIndex(nCurveModel);
    m_pGridTable->setCellWidget(nRow , nCount + 2 , pComboBox);
    connect(pComboBox , SIGNAL(currentIndexChanged(int)) , this , SLOT(slotMoveCurvesChange(int)));

    QTableWidgetItem* pFirstItem = m_pGridTable->item(nRow,0);
    pFirstItem->setData(ActionBlockDataID, ActionBlockID);

    m_pGridTable->setRowHeight(nRow, GRID_COL_HEIGHT);
    return nRow;
}

void COptionsMotionWidget::UpdateTableWidgetShow(int nRow, CActionBlockData *pActionBlock)
{
    if(pActionBlock == NULL ||
            m_pRobotView == NULL ||
            m_pGridTable == NULL)
        return;

    char* pData = NULL;
    int nLen = 0;
    pActionBlock->GetExtBlockData(&pData, nLen);
    int nCount = (nLen - sizeof(nLen))/sizeof(MOTOR_DATA);
    for(int i = 0; i < nCount; i++)
    {
        MOTOR_DATA* pMotor = GetMotorData(pData, nLen, i);
        QTableWidgetItem* item = m_pGridTable->item(nRow, i);
        if (item)
        {
            item->setText(QString::number(pMotor->nAngle));
        }
    }
}

void COptionsMotionWidget::on_pbModify_clicked()
{
    if(m_pGridTable == NULL || m_pGroupManager == NULL)
        return;

    CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
    if(pGroupManager == NULL)
        return;

    for(int i = 0; i<m_pGridTable->rowCount();++i)
    {
        int nRow = i;
        if( (nRow >= 0) && IsFullRowSelect(nRow) )
        {
            QTableWidgetItem * pItem = m_pGridTable->item(i,0);
            int nBlockDataID = pItem->data(ActionBlockDataID).toInt();
            if(m_pEvent && m_pRobotView)
            {
                char* pData = NULL;
                int nLen = 0;
                m_pRobotView->GetMotorDatas(&pData, nLen, m_eActionType);
                if(pData && nLen)
                {
                    CActionBlockData* pBlockData = pGroupManager->FindBlockData(nBlockDataID);
                    if (pBlockData == NULL)
                        return;
                    pBlockData->SetExtBlockData(nLen, pData);
                    int nCurrentID = pBlockData->GetID();
                    SetShowID(&nCurrentID, 1);
                    if(m_pEvent)
                    {
                        m_pEvent->ILayerOptionHandler_OnActionBlockDataChange(m_nCurrentGroupID, nCurrentID, 0);
                    }
                    UpdateTableWidgetShow(i, pBlockData);
                }
            }
        }
    }

}


void COptionsMotionWidget::onMotionDelete()
{
    if(m_pGridTable == NULL ||
            m_pGroupManager == NULL)
    {
        qDebug()<<"can't find m_pGridTable"<<endl;
        return;
    }

    CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
    if(pGroupManager == NULL)
        return;

    bool bUpdate = false;
    // 最后一次删除的ID
    int nLastDeleteID = -1;
    int nTimeInc = 0;

    for(int i = m_pGridTable->rowCount() - 1; i >= 0 ; i--)
    {
        bool bSelectFullLine = true;
        bSelectFullLine = IsFullRowSelect(i);

        if (bSelectFullLine)
        {
            bUpdate = true;

            QTableWidgetItem * pItem = m_pGridTable->item(i,0);
            int nActionBlockID = pItem->data(ActionBlockDataID).toInt();
            CActionBlockData* pActionBlockData = pGroupManager->FindBlockData(nActionBlockID);
            if(pActionBlockData == NULL)
                continue;

            nTimeInc += (pActionBlockData->GetStopTime() + pActionBlockData->GetRunTime());
            nLastDeleteID = i;

            pGroupManager->DeleteLayerBlock(nActionBlockID);

            if (pGroupManager->GetActionCount() < 1) //判断是否还有数据
            {
                int nStartTime = pGroupManager->GetStartTime();
                m_pEvent->ILayerOptionHandler_SetSelectGroupBlock(nStartTime + 2);
                m_pEvent->ILayerOptionHandler_OnDeleteGroupBlock();
            }

        } else {
            continue;
        }
    }

    // 滚动到最后一条删除的记录
//    if (nLastDeleteID != -1)
//    {
//        if (nLastDeleteID < m_pGridTable->rowCount()-1)
//        {
//            int nShowID = m_pGridTable->item(nLastDeleteID+1, 0)->data(ActionBlockDataID).toInt();
//            CActionBlockData* pActionBlockData = pGroupManager->FindBlockData(nShowID);

//            if (pActionBlockData)
//            {
//                nShowID = pActionBlockData->GetID();
//            }
//            else
//            {
//                nShowID = -1;
//            }

//            nLastDeleteID = nShowID;
//        }
//        else
//        {
//            nLastDeleteID = -1;
//        }
//    }

    if (bUpdate)
    {
        int nCurrentID = m_nCurrentGroupID;
        m_nCurrentGroupID = -1;
        g_IsUpdateUIByData = true;
        m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, -nTimeInc, true);
        g_IsUpdateUIByData = false;

        if (nLastDeleteID != -1)
        {
            SetShowID(&nLastDeleteID, 1);
            ShowSelectRow(nLastDeleteID);
        }
        //            else
        //            {
        //                // 滚动到最后一行
        //                if (m_Grid.GetRowCount())
        //                    m_Grid.EnsureVisible(m_Grid.GetRowCount()-1, 0);
        //            }
    }
}


bool COptionsMotionWidget::IsFullRowSelect(int nRow)
{
    if(m_pGridTable == NULL)
        return false;
    //for (int j=1; j<m_pGridTable->columnCount(); ++j)
    //{

    //    QTableWidgetItem * ptwiActionItem = m_pGridTable->item(nRow,j);
    //    if (m_pGridTable->isItemSelected(ptwiActionItem) == false)    //判断是否选中一行
    //    {
    //        return false;
    //    }
    //}

    // Modify by jianjie 2017/10/11
    QTableWidgetItem * ptwiActionItem = m_pGridTable->item(nRow , 0);
    if (m_pGridTable->isItemSelected(ptwiActionItem) == true)    //判断是否选中一行
    {
        return true;
    }

    return false;
}


bool COptionsMotionWidget::IsTableHasSelectRow()
{
    if(m_pGridTable == NULL)
        return false;
    for(int i=0; i<m_pGridTable->rowCount(); i++)
    {
        if (IsFullRowSelect(i) == true)
            return true;
    }

    return false;
}


void COptionsMotionWidget::ShowSelectRow(int row)
{
    if(m_pGridTable)
    {
        m_pGridTable->selectRow(row);
    }
}

void COptionsMotionWidget::initTableWidget()
{
    //    QTableWidget *table =ui->twActionList;
    if(m_pGridTable == NULL)
    {
        qDebug()<<"m_pGridTable is NULL"<<endl;
        return;
    }

    int nCfgMotorCount = CConfigs::getCfgMotorCount(); //配置的舵机数

    int nStartMotorID = 1;

    switch (m_eActionType)
    {
    case ACTION_LAYER_HAND:
        {
            nCfgMotorCount  = HAND_MOTOR_COUNT;
            nStartMotorID   = 1;
        }
    	break;
    case ACTION_LAYER_LEG:
        {
            nCfgMotorCount  = LEG_MOTOR_COUNT;
            nStartMotorID   = 1 + HAND_MOTOR_COUNT;
        }
        break;
    case ACTION_LAYER_HEAD:
        {
            nCfgMotorCount  = HEAD_MOTOR_COUNT;
            nStartMotorID   = 1 + HAND_MOTOR_COUNT + LEG_MOTOR_COUNT;
        }
        break;
    default:
        break;
    }

    int nColumnCount = nCfgMotorCount + 3;
    m_nRunTimeCol       = nColumnCount - 3;
    m_nStopTimeCol      = nColumnCount - 2;
    m_nMoveCurvesCol    = nColumnCount - 1;

    m_pGridTable->setColumnCount(nColumnCount);
    //    table->setRowCount();
    for(int i = 0; i < nColumnCount; i++)
    {
        if(i < m_nRunTimeCol)
            m_pGridTable->setColumnWidth(i, GRID_COL_WIDTH);
        else
            m_pGridTable->setColumnWidth(i, 80);
    }
    //m_pGridTable->setColumnWidth(m_pGridTable->columnCount()-1,80);
    //m_pGridTable->setColumnWidth(m_pGridTable->columnCount(),80);

    //设置表格行标题
    QStringList headerLabels;
    for(int i =0; i < nCfgMotorCount; i++)
    {
        QString strTitle = "ID"+QString::number(nStartMotorID + i);
        headerLabels << strTitle;
    }
    headerLabels<<tr("runTime")<<tr("stopTime")<<tr("move Curves");
    m_pGridTable->setHorizontalHeaderLabels(headerLabels);

    m_pGridTable->setFrameShape(QFrame::NoFrame);
    m_pGridTable->setShowGrid(false); //不显示格子线
    m_pGridTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    m_pGridTable->horizontalHeader()->setFixedHeight(GRID_COL_HEIGHT);
    m_pGridTable->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    m_pGridTable->verticalHeader()->setFixedWidth(GRID_COL_WIDTH);

    //    //设置表格行标题的对齐方式
    //    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    //    //设置行表题
    //    QStringList rowLabels;
    //    rowLabels << "Line1" << "Line2" << "Line3" << "Line4";
    //    table->setVerticalHeaderLabels(rowLabels);


    //    //设置每一行的高度
    //    for(int i = 0; i < 4; i++)
    //        table->setRowHeight(i, 22);

    //自动调整最后一列的宽度使它和表格的右边界对齐
    //    table->horizontalHeader()->setStretchLastSection(true);

    // 设置表格可选中多个目标
    m_pGridTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

    //设置表格的选择方式
    m_pGridTable->setSelectionBehavior(QAbstractItemView::SelectItems);

    //设置编辑方式
    m_pGridTable->setEditTriggers(QAbstractItemView::DoubleClicked);
}


void COptionsMotionWidget::InsertSampleListToTableWidget(int nCurRow)
{


    int cols=m_pGridTable->columnCount();
    //           int rows=ui->twActionList->rowCount();
    //           qDebug()<<rows;
    m_pGridTable->insertRow(nCurRow);
    QTableWidgetItem* pItem = NULL;
    for(int i=0;i<cols;i++)
    {
        pItem = new QTableWidgetItem(QString::number(nCurRow+1));
        pItem->setTextAlignment(Qt::AlignCenter);
        m_pGridTable->setItem(nCurRow,i,pItem);
    }
    m_pGridTable->setRowHeight(nCurRow, GRID_COL_HEIGHT);
    m_pGridTable->selectRow(nCurRow);
}


bool COptionsMotionWidget::CopySampleSelsctItem()
{

    //    bool bRslt =false;
    QTableWidgetItem * item =  m_pGridTable->currentItem();
    if(item == NULL)
    {
        return false;
    }
    int nRow = item->row();
    for (int j=0; j<m_pGridTable->columnCount(); j++)
    {
        QTableWidgetItem * ptwiActionItem = m_pGridTable->item(nRow,j);
        if (m_pGridTable->isItemSelected(ptwiActionItem) ==false)    //判断是否选中一行
        {
            //            bRslt = false ;de
            return false;
        }
        QString strText = ptwiActionItem->text();
    }
    return true;
}


void COptionsMotionWidget::UpdateUIByData()
{
    if(m_pGroupManager ==NULL)
    {
        return;
    }
    if(m_pGroupListWidget ==NULL)
    {
        return;
    }
    if((m_pGroupListWidget->count() == m_pGroupManager->GetCount()) && !g_IsModifyGroup)
    {
        m_pGroupListWidget->setItemSelected(m_pGroupListWidget->item(m_nCurrentGroupListSelectItem),true);
        return;
    }
    m_pGroupListWidget->clear();
    for(int i = 0;i<m_pGroupManager->GetCount();i++)
    {
        CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(i);
        QString strDescription = QString::fromStdWString(pGroup->GetDescription());
        //QString strDescription = QString::fromUtf16(pGroup->GetDescription());
        QListWidgetItem * pItem = new QListWidgetItem(strDescription);
        m_pGroupListWidget->insertItem(i,pItem);
        int nID = pGroup->GetID();
        pItem->setData(GroupID,nID);
    }
    m_pGroupListWidget->show();

    m_pEvent->ILayerOptionHandler_HasMotionData(m_pGroupListWidget->count() > 0, ACTION_LAYER_MOTION);
    if(m_pGroupListWidget->count() > 0)
    {
//        ui->btnUnitedSimulation->setEnabled(true);
//        ui->btnStop->setEnabled(true);
        ui->pbInsert->setEnabled(true);
        ui->pbModify->setEnabled(true);
    }
    else
    {
//        ui->btnUnitedSimulation->setEnabled(false);
//        ui->btnStop->setEnabled(false);
        ui->pbInsert->setEnabled(false);
        ui->pbModify->setEnabled(false);
    }
}


void COptionsMotionWidget::SetCurrentSelectItem(QListWidgetItem * item, int nIndex)
{
    if(m_pGroupListWidget == NULL)
    {
        return;
    }
    if(m_nCurrentGroupListSelectItem !=-1)
    {
        m_pGroupListWidget->setItemSelected(item, true);//此处Windows版加了图标
    }
    if(nIndex == -1)
    {
        if(m_pGridTable->winId())
        {
            m_pGridTable->clearContents();
            m_pGridTable->setRowCount(0);
        }
        return;
    }
    m_pGroupListWidget->setItemSelected(item,true);
    m_nCurrentGroupListSelectItem = nIndex;
    if(m_pGroupManager)
    {
        CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(nIndex);
        if(pGroup)
        {
            m_nCurrentGroupID = pGroup->GetID();
            UpdataGridByGroupManager(pGroup);
        }
    }

}

void COptionsMotionWidget::UpdataGridByGroupManager(CActionBlockGroupManager *pGroup)
{
    if(m_pGridTable == NULL)
    {
        return;
    }
    m_pGridTable->clearContents();
    m_pGridTable->setRowCount(0);
    int nRow = -1;
    for(int i = 0; i<pGroup->GetActionCount(); i++)
    {
        CActionBlockData* pBlockData = pGroup->GetAt(i);
        if(pBlockData)
        {
            char* pData = NULL;
            int nLen = 0;
            pBlockData->GetExtBlockData(&pData, nLen);

            //QString strDestription = WCHARToQString(pBlockData->GetDescription(), MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
            //QString strDescription = QString::fromUtf16(pBlockData->GetDescription());
            QString strDescription = QString::fromStdWString(pBlockData->GetDescription());
            if(pData && nLen)
            {
                nRow = AddToTableWidget(pData, nLen, strDescription, pBlockData->GetRunTime(),
                                        pBlockData->GetStopTime(), pBlockData->GetID());
                delete[] pData;
            }
        }
//        ui->leBeatValue->setEnabled(true);
    }
    ShowSelectRow(nRow);
}

void COptionsMotionWidget::deleteSampleListItem(int nRow)
{
    //    QTableWidgetItem * item = ui->tableWidget->currentItem();
    //    if(item==Q_NULLPTR)return;
    //    int nRow = item ->row();
    if(nRow>=0)
        m_pGridTable->removeRow(nRow);
    //    if(ui->)
    //     ui->twActionList->selectRow(nRow-1);
}


//void COptionsMotionWidget::on_btnAddGroup_clicked()
//{
//    //    AddActionGroup();

//    SetActionBlockDataManager(m_pGroupManager, -1,-1);
//    //    if(m_pEvent)
//    //    {
//    //        m_pEvent->ILayerOptionHandler_OnSelectGroupManageChange(-1,-1);

//    //    }
//    int nScaleTime = 10 ;//时间单位（ms）
//    if (m_pEvent && m_pRobotView)
//    {
//        char* pData = NULL;
//        int nLen = 0;

//        m_pRobotView->GetMotorDatas(&pData, nLen);
//        if (pData && nLen)
//        {
//            int nRunTime = 0;
//            QString tempValue;
//            tempValue = ui->leBeatValue->text();
//            nRunTime = tempValue.toInt();
//            if (nRunTime < 10)
//            {
//                nRunTime = 10;
//            }
//            CActionBlockData* pActionBlockData = new CActionBlockData();
//            pActionBlockData->SetID(-1);
//            WCHAR wstrDescription[MAX_DESCRIPTION_SIZE];
//            memset(wstrDescription,0,MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
//            QString strDescription = QObject::tr("Action");
//            int nDescriptionLen = QStringToWCHAR(wstrDescription, strDescription);
//            wstrDescription[nDescriptionLen-1] ='\0';
//            pActionBlockData->SetDescription(wstrDescription);
//            pActionBlockData->SetRunTime(nRunTime / nScaleTime);
//            pActionBlockData->SetAllTime(nRunTime / nScaleTime);//设置总时间
//            pActionBlockData->SetExtBlockData(nLen, pData);
//            int nCurrentID = GetCurrenGroupManagerID();
//            g_IsUpdateUIByData = true;   //这里默认需要修改
//            m_pEvent->ILayerOptionHandler_OnAddActionBlock(nCurrentID, pActionBlockData);
//            //                       float fAllTime = nRunTime*(nID+1);
//            if (nCurrentID != -1)
//            {
//                int nID = pActionBlockData->GetID();

//                SetShowID(&nID, 1);
//                QString strTmpDescription = WCHARToQString(pActionBlockData->GetDescription(), MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
//                AddToTableWidget(pData, nLen, strTmpDescription, pActionBlockData->GetRunTime(), pActionBlockData->GetAllTime(), nID);
//                //                            ShowSelectActionBlock();
//            }
//            else
//            {
//                int nID = pActionBlockData->GetID();
//                SetShowID(&nID, 1);
//                //                            ShowSelectActionBlock();
//            }
//            //            delete [] pData;
//        }
//    }

//}


//void COptionsMotionWidget::on_m_ListActionGroup_clicked(const QModelIndex &index)
//{
//    int nRow = index.row();
//    if(nRow != -1 )
//    {
//        SetCurrentSelectItem(m_pGroupListWidget->item(nRow),nRow);
//        if(m_pEvent)
//        {
//            m_pEvent->ILayerOptionHandler_OnSelectGroupManageChange(m_nCurrentGroupID,0);
//        }
//    }
//}


//void COptionsMotionWidget::on_m_ListActionGroup_itemClicked(QListWidgetItem *item)
//{
//    if(item == NULL)
//    {
//        return;
//    }
//    SetCurrentSelectItem(item, item->);
//}


//void COptionsMotionWidget::on_btnDeleteGroup_clicked()
//{
//    if(m_pGroupListWidget == NULL ||
//            m_pEvent == NULL)
//    {
//        return;
//    }
//    QList<QListWidgetItem*> listItems = m_pGroupListWidget->selectedItems();
//    for(int i = 0; i < listItems.count(); i++)
//    {
//        QListWidgetItem* pItem = listItems.at(i);
//        int nGroupID = pItem->data(GroupID).toInt();
//        CActionBlockGroupManager* pGroup = m_pGroupManager->FindBlockData(nGroupID);
//        if(pGroup)
//        {
//            int startTime = pGroup->GetStartTime();
//            m_pEvent->ILayerOptionHandler_SetSelectGroupBlock(startTime + 2);
//        }
//    }

//    m_pEvent->ILayerOptionHandler_OnDeleteGroupBlock();

//    if(m_pGridTable ==NULL)
//    {
//        return;
//    }
//    for(int i = m_pGridTable->rowCount(); i>=0;--i)
//    {
//        deleteSampleListItem(i);
//    }
//}

void COptionsMotionWidget::on_m_ListActionGroup_itemPressed(QListWidgetItem *item)
{
    if(qApp->mouseButtons() == Qt::LeftButton)
    {
        int nRow = m_pGroupListWidget->row(item);
        if(nRow != -1 )
        {
            SetCurrentSelectItem(item,nRow);
            if(m_pEvent)
            {
                m_pEvent->ILayerOptionHandler_OnSelectGroupManageChange(m_nCurrentGroupID,0);
            }
        }
    }
    else if(qApp->mouseButtons() == Qt::RightButton) //右键
    {
        if(item==Q_NULLPTR)
        {
            return;
        }
        else
        {
            int nRow = m_pGroupListWidget->row(item);
            if(nRow != -1 )
            {
                SetCurrentSelectItem(item,nRow);
                if(m_pEvent)
                {
                    m_pEvent->ILayerOptionHandler_OnSelectGroupManageChange(m_nCurrentGroupID,0);
                }
            }

            CreateGroupListPopMenuActions();
        }

    }
}


void COptionsMotionWidget::CreateGroupListPopMenuActions()
{
    //创建菜单、菜单项
    QMenu* pop_menu = new QMenu();
    QAction* DeleteAction = new QAction(tr("Delete"),this);
    QAction* ModifyAction = new QAction(tr("Modify group name"),this);
    QAction* CopyAction = new QAction(tr("Copy"),this);
    QAction* PasteAction = new QAction(tr("Paste"),this);

    pop_menu->addAction(DeleteAction);
    pop_menu->addAction(ModifyAction);
    pop_menu->addAction(CopyAction);
    pop_menu->addAction(PasteAction);
    QAbstractItemDelegate::connect(DeleteAction,&QAction::triggered,this,&COptionsMotionWidget::onGroupListDelete);
    QAbstractItemDelegate::connect(ModifyAction,&QAction::triggered,this,&COptionsMotionWidget::onGroupModifyName);
    QAbstractItemDelegate::connect(CopyAction,&QAction::triggered,this,&COptionsMotionWidget::onGroupListCopy);
    QAbstractItemDelegate::connect(PasteAction,&QAction::triggered,this,&COptionsMotionWidget::onGroupListPaste);
    //菜单出现的位置为当前鼠标的位置
    QFile styleSheet(":/res/qss/menudefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        pop_menu->setStyleSheet(styleSheet.readAll());
    }
    pop_menu->exec(QCursor::pos());
    pop_menu->deleteLater();
    //连接信号与槽
}


void COptionsMotionWidget::CreateGridListPopMenuActions()
{
    //创建菜单、菜单项
    QMenu* pop_menu = new QMenu();
    QAction* CopyAction = new QAction(tr("Copy"),this);
    QAction* InsertCopyAction = new QAction(tr("Insert copied item"),this);
    QAction* AddCopyAction = new QAction(tr("Add replication entry"),this);
    QAction* AddMirrorAction = new QAction(tr("Add mirror item"), this);
    QAction* BatchModify = new QAction(tr("Batch Modify"),this);
    QAction* DeleteAction = new QAction(tr("Delete"),this);

    pop_menu->addAction(CopyAction);
    pop_menu->addAction(InsertCopyAction);
    pop_menu->addAction(AddCopyAction);
    pop_menu->addAction(AddMirrorAction);
    pop_menu->addAction(BatchModify);
    pop_menu->addAction(DeleteAction);

    //连接信号与槽
    QAbstractItemDelegate::connect(CopyAction,&QAction::triggered,this,&COptionsMotionWidget::onMotionCopy);
    QAbstractItemDelegate::connect(InsertCopyAction,&QAction::triggered,this,&COptionsMotionWidget::onMotionInsert);
    QAbstractItemDelegate::connect(AddCopyAction,&QAction::triggered,this,&COptionsMotionWidget::onMotionPaste);
    QAbstractItemDelegate::connect(AddMirrorAction, &QAction::triggered, this, &COptionsMotionWidget::onMotionMirror);
    QAbstractItemDelegate::connect(BatchModify,&QAction::triggered,this,&COptionsMotionWidget::onBatchModify);
    QAbstractItemDelegate::connect(DeleteAction,&QAction::triggered,this,&COptionsMotionWidget::onMotionDelete);

    int nCurRow = m_pGridTable->currentRow();
    bool bSelectFullLine = IsFullRowSelect(nCurRow);
    if(!bSelectFullLine)
    {
        CopyAction->setEnabled(false);
        InsertCopyAction->setEnabled(false);
        DeleteAction->setEnabled(false);
        AddMirrorAction->setEnabled(false);
    }

    const QClipboard *clipboard = QApplication::clipboard();
    const  QMimeData* mimeData = clipboard->mimeData();
    if(mimeData != NULL)
    {
        QByteArray bArray = mimeData->data("CopyData");
        int nTotalLen = bArray.length();
        if(nTotalLen <= 0)
        {
            InsertCopyAction->setEnabled(false);
            AddCopyAction->setEnabled(false);
        }
    }

    //菜单出现的位置为当前鼠标的位置
    QFile styleSheet(":/res/qss/menudefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        pop_menu->setStyleSheet(styleSheet.readAll());
    }
    pop_menu->exec(QCursor::pos());
    pop_menu->deleteLater();
}


void COptionsMotionWidget::onGroupListDelete()
{
    //    QListWidgetItem* pItem = m_pGroupListWidget->currentItem();
    int nIndex = m_pGroupListWidget->currentRow();
    if(nIndex == -1)
    {
        return;
    }
    else
    {
        //        int nIndex = pItem->data(Qt::DecorationRole).toInt();
        CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(nIndex);
        if(pGroup == NULL)
        {
            qDebug()<<tr("new pGroup Fail in COptionsMotionWidget class")<<endl;
            return;
        }
        int nStartTime = pGroup->GetStartTime();
        m_pEvent->ILayerOptionHandler_SetSelectGroupBlock(nStartTime + 2);
    }
    m_pEvent->ILayerOptionHandler_OnDeleteGroupBlock();
    m_pEvent->ILayerOptionHandler_HasMotionData(m_pGroupListWidget->count() > 0, ACTION_LAYER_MOTION);
    if(m_pGroupListWidget->count() > 0)
    {
//        ui->btnUnitedSimulation->setEnabled(true);
//        ui->btnStop->setEnabled(true);
        ui->pbInsert->setEnabled(true);
        ui->pbModify->setEnabled(true);
    }
    else
    {
//        ui->btnUnitedSimulation->setEnabled(false);
//        ui->btnStop->setEnabled(false);
        ui->pbInsert->setEnabled(false);
        ui->pbModify->setEnabled(false);
    }
}

void COptionsMotionWidget::onGroupModifyName()
{
    DialogCommandBox dlg;
    QFile styleSheet(":/res/qss/motiondefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        dlg.setStyleSheet(styleSheet.readAll());
    }
    if (dlg.exec() == QDialog::Accepted)
    {
        QString strDescription = dlg.m_strCommandBoxName;
        if (strDescription.isEmpty())
        {
            return;
        }

        std::wstring wsDescription = strDescription.toStdWString();
        CActionBlockGroupManager* pGroup = m_pGroupManager->FindBlockData(GetCurrenGroupManagerID());
        if(pGroup)
        {
            pGroup->SetDescription((WCHAR *)(wsDescription.c_str()));
        }
        if (m_pEvent)
        {
            m_pEvent->ILayerOptionHandler_OnGroupManagerChange(m_nCurrentGroupID, 0, true);
        }

        g_IsModifyGroup = true;
        UpdateUIByData();
    }

//    CActionBlockGroupManager* pGroup = m_pGroupManager->FindBlockData(GetCurrenGroupManagerID());
//    if(pGroup)
//    {
//        int nStartTime = pGroup->GetStartTime();
//        m_pEvent->ILayerOptionHandler_ModifyGroupName(nStartTime +1);
//    }
}



void COptionsMotionWidget::onGroupListCopy()
{
    m_pEvent->ILayerOptionHandler_CopyGroup();
}



void COptionsMotionWidget::onGroupListPaste()
{
    CActionBlockGroupManager* pGroup = m_pGroupManager->FindBlockData(GetCurrenGroupManagerID());
    if(pGroup)
    {
        int nStartTime = pGroup->GetStartTime();
        m_pEvent->ILayerOptionHandler_PasteGroup(nStartTime + 1);
    }
}



void COptionsMotionWidget::on_twActionList_itemPressed(QTableWidgetItem *item)
{
    if(qApp->mouseButtons() == Qt::RightButton) //右键
    {
        if(item==Q_NULLPTR)
        {
            return;
        }
        else
        {

            CreateGridListPopMenuActions();
        }

    }
}


void COptionsMotionWidget::onSectionDoubleClicked(int logicalIndex)
{
    if(    NULL == m_pGroupManager
        || NULL == m_pGridTable
        ||   -1 == m_nCurrentGroupListSelectItem)
        return;

    QTableWidgetItem* pItem = m_pGridTable->item(logicalIndex, 0);
    if(NULL == pItem)
        return;
    int nActionBlockID = pItem->data(ActionBlockDataID).toInt();
    if(nActionBlockID >= 0)
    {
        CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(m_nCurrentGroupListSelectItem);
        if(pGroup == NULL)
        {
            return;
        }

        if(m_pEvent)
        {
            m_pEvent->ILayerOptionHandler_OnSelectActionManageChange(pGroup->GetID(), logicalIndex);
        }

        ModleView(nActionBlockID);
    }
    m_ShowId.clear();
}

//hels 响应编辑Item信号
void COptionsMotionWidget::onItemCommitData(QWidget *editor)
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(m_pGridTable && lineEdit)
    {
        int nScaleTime = 10;//时间单位
        if (m_pEvent)
        {
            nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();
        }

        int nRow = m_pGridTable->currentRow();
        int nColumn = m_pGridTable->currentColumn();

        QTableWidgetItem* pItem0 = m_pGridTable->item(nRow, 0);
        int nActionBlockID = pItem0->data(ActionBlockDataID).toInt();
        CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
        if (pGroupManager)
        {
            QTableWidgetItem* pItem = m_pGridTable->item(nRow, nColumn);
            CActionBlockData* pBlockData = pGroupManager->FindBlockData(nActionBlockID);
            if (pBlockData)
            {
                // 舵机数据
                if (nColumn < m_nRunTimeCol)
                {
                    QString strAngle = lineEdit->text();
                    if (strAngle.isEmpty())
                    {
                        strAngle.sprintf("%d",GRID_MIN_ANGLEVALUE);
                        pItem->setText(strAngle);
                    }
                    bool bOk = false;
                    int nAngle = strAngle.toInt(&bOk);
                    if (nAngle > GRID_MAX_ANGLEVALUE || bOk == false)
                    {
                        // 数据不合格，恢复数据
                        char* pData = NULL;
                        int nLen = 0;
                        pBlockData->GetExtBlockData(&pData, nLen);
                        if (pData && nLen)
                        {
                            MOTOR_DATA* pMoto = GetMotorData(pData, nLen, nColumn);
                            if (pMoto)
                            {
                                strAngle.sprintf("%d", pMoto->nAngle);
                                pItem->setText(strAngle);
                            }
                            delete [] pData;
                        }
                    }
                    else
                    {
                        // 更新最新的数据
                        char* pData = NULL;
                        int nLen = 0;
                        pBlockData->GetExtBlockData(&pData, nLen);
                        if (pData && nLen)
                        {
                            // Modified Flag
                            SetMotorAngle(pData, nLen, nColumn, nAngle);
                            if (m_pEvent)
                            {
                                m_pEvent->ILayerOptionHandler_SetModifiedState(true);
                            }
                            pBlockData->SetExtBlockData(nLen, pData);
                            delete [] pData;
                        }
                    }
                }
                else if (nColumn == m_nRunTimeCol
                         || nColumn == m_nStopTimeCol)//修改了运动时间或停止时间
                {
                    int nTimeInc = 0;
                    bool bIsRunTimeCol = (nColumn == m_nRunTimeCol);

                    QString sTempTime = lineEdit->text();
                    if (sTempTime.isEmpty())
                    {
                        sTempTime.sprintf("%d", GRID_MIN_TIMEVALUE);
                        pItem->setText(sTempTime);
                    }

                    bool bOk = false;
                    int nRealTime = sTempTime.toInt(&bOk);

                    //无效数据
                    if (!bOk || nRealTime > GRID_MAX_TIMEVALUE || nRealTime < GRID_MIN_TIMEVALUE)
                    {
                        int nTempTime = (bIsRunTimeCol ? pBlockData->GetRunTime() : pBlockData->GetStopTime());
                        sTempTime.sprintf("%d", nTempTime);
                        pItem->setText(sTempTime);
                    }
                    else
                    {
                        if (bIsRunTimeCol)
                        {
                            nTimeInc = nRealTime - pBlockData->GetRunTime();
                            pBlockData->SetRunTime(nRealTime);

                        }
                        else
                        {
                            nTimeInc = nRealTime - pBlockData->GetStopTime();
                            pBlockData->SetStopTime(nRealTime);
                        }
                        pItem->setText(sTempTime);
                    }
                    if(m_pEvent)
                    {
                        m_pEvent->ILayerOptionHandler_OnActionBlockDataChange(m_nCurrentGroupID, nActionBlockID, nTimeInc);//分组总时间改变通知
                    }

//                    QString sRunTime = m_pGridTable->item(nRow, m_nRunTimeCol)->text();
//                    QString sStopTime = m_pGridTable->item(nRow, m_nStopTimeCol)->text();

//                    if(sRunTime.toInt() > sAllTime.toInt())
//                    {
//                        if(bIsRunTimeCol)
//                        {
//                            if(sRunTime.toInt() <= GRID_MAX_TIMEVALUE)
//                            {
//                                QTableWidgetItem* pItemAllTime = m_pGridTable->item(nRow, m_nStopTimeCol);
//                                pItemAllTime->setText(sRunTime);//设置总时间为运行时间
//                            }
//                            else
//                            {
//                                //四舍五入
//                                int nTempTime = pBlockData->GetRunTime() * nScaleTime + 0.5;
//                                sTempTime.sprintf("%d", nTempTime);
//                                pItem->setText(sTempTime);
//                            }
//                        }
//                        else
//                        {
//                            if(sAllTime.toInt() <= GRID_MAX_TIMEVALUE)
//                            {
//                                QTableWidgetItem* pItemRunTime = m_pGridTable->item(nRow, m_nRunTimeCol);
//                                pItemRunTime->setText(sAllTime);//设置运行时间为总时间
//                            }
//                            else
//                            {
//                                //四舍五入
//                                int nTempTime = pBlockData->GetAllTime() * nScaleTime + 0.5;
//                                sTempTime.sprintf("%d", nTempTime);
//                                pItem->setText(sTempTime);
//                            }
//                        }
//                    }

//                    //前面判断运行时间和总时间时有更改数据，因此这里重新获取一次
//                    sRunTime = m_pGridTable->item(nRow, m_nRunTimeCol)->text();
//                    sAllTime = m_pGridTable->item(nRow, m_nStopTimeCol)->text();


//                    int nRunTime = sRunTime.toInt();
//                    int nAllTime = sAllTime.toInt();
//                    int nAllTimeInc = nAllTime / nScaleTime - pBlockData->GetAllTime();//改变的时间

//                    pBlockData->SetRunTime(nRunTime / nScaleTime);
//                    pBlockData->SetAllTime(nAllTime / nScaleTime);
                }
            }
        }
    }
}

void COptionsMotionWidget::OnMotionPlayStop()
{
    //ui->btnUnitedSimulation->setText(tr("United Simulation"));
//    ui->btnPause->hide();
//    ui->btnUnitedSimulation->show();
//    m_bIsTotalPlaying = false;
//    ui->musicSlider->setSliderPosition(0);
}

void COptionsMotionWidget::OnSetControlState(bool controlState, int nLayerType)
{
    if(controlState)
    {
//        ui->leBeatValue->setEnabled(true);
        ui->m_ListActionGroup->setEnabled(true);
        ui->twActionList->setEnabled(true);
        ui->pbAdd->setEnabled(true);
        ui->pbInsert->setEnabled(true);
        ui->pbModify->setEnabled(true);
    }
    else
    {
//        ui->leBeatValue->setEnabled(false);
        ui->m_ListActionGroup->setEnabled(false);
        ui->twActionList->setEnabled(false);
        ui->pbAdd->setEnabled(false);
        ui->pbInsert->setEnabled(false);
        ui->pbModify->setEnabled(false);
    }
}

void COptionsMotionWidget::on_btnUnitedSimulation_clicked()
{
    if(m_bIsTotalPlaying)
    {
        //ui->btnUnitedSimulation->setText(tr("United Simulation"));
//        ui->btnPause->hide();
//        ui->btnUnitedSimulation->show();
        if(m_pEvent)
        {
            m_pEvent->ILayerOptionHanlder_PauseUnitedSimulation();
        }
        m_bIsTotalPlaying = false;
    }
    else
    {
        //ui->btnUnitedSimulation->setText(tr("Pause"));
//        ui->btnUnitedSimulation->hide();
//        ui->btnPause->show();
        if(m_pEvent)
        {
            m_pEvent->ILayerOptionHanlder_UnitedSimulation();
        }
        m_bIsTotalPlaying = true;
    }
}

void COptionsMotionWidget::on_btnStop_clicked()
{
    if(m_pEvent)
    {
        //ui->btnUnitedSimulation->setText(tr("United Simulation"));
//        ui->btnPause->hide();
//        ui->btnUnitedSimulation->show();
        m_pEvent->ILayerOptionHanlder_StopUnitedSimulation();
        m_bIsTotalPlaying = false;
    }
}

//wanxf  在动作帧列表批量修改列数据
void COptionsMotionWidget::onBatchModify()
{
    QString strAngle;
    BatchModifyDialog dlg;
    QFile styleSheet(":/res/qss/motiondefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        dlg.setStyleSheet(styleSheet.readAll());
    }
    if (dlg.exec() == QDialog::Accepted)
    {
        strAngle = dlg.m_strModifyValue;
    } else {
        return;
    }

    int nModifyValue = strAngle.toInt();
    QList<QTableWidgetItem*> items = m_pGridTable->selectedItems();
    int nCount = items.count();
    for(int i = 0; i < nCount; i++)
    {
       int nRow = m_pGridTable->row(items.at(i));//获取选中的行
       int nColumn = m_pGridTable->column(items.at(i));//获取选中的列
       QTableWidgetItem* pItem0 = m_pGridTable->item(nRow, 0);
       int nActionBlockID = pItem0->data(ActionBlockDataID).toInt();

       CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
       if (pGroupManager)
       {           
           CActionBlockData* pBlockData = pGroupManager->FindBlockData(nActionBlockID);
           if (pBlockData)
           {
               // 更新最新的数据
               if (nColumn < m_nRunTimeCol)
               {
                   if (nModifyValue > GRID_MAX_ANGLEVALUE)
                       return;
                   char* pData = NULL;
                   int nLen = 0;
                   pBlockData->GetExtBlockData(&pData, nLen);
                   if (pData && nLen)
                   {
                       // Modified Flag
                       SetMotorAngle(pData, nLen, nColumn, nModifyValue);
                       if (m_pEvent)
                       {
                           m_pEvent->ILayerOptionHandler_SetModifiedState(true);
                       }
                       pBlockData->SetExtBlockData(nLen, pData);
                       delete [] pData;
                   }
               } else {
                   if (nModifyValue > GRID_MAX_TIMEVALUE)
                       return;
                   int nTimeInc = 0;
                   bool bIsRunTimeCol = (nColumn == m_nRunTimeCol);
                   if (bIsRunTimeCol)
                   {
                      nTimeInc = nModifyValue - pBlockData->GetRunTime();
                      pBlockData->SetRunTime(nModifyValue);

                   } else {
                      nTimeInc = nModifyValue - pBlockData->GetStopTime();
                      pBlockData->SetStopTime(nModifyValue);
                   }

                   if(m_pEvent)
                   {
                      m_pEvent->ILayerOptionHandler_OnActionBlockDataChange(m_nCurrentGroupID, nActionBlockID, nTimeInc);//分组总时间改变通知
                   }
               }
           }
           QTableWidgetItem* pItem = m_pGridTable->item(nRow, nColumn);
           pItem->setText(strAngle);
       }
    }
}


void COptionsMotionWidget::slotMoveCurvesChange(int nCurveModel)
{
    QComboBox* pSender = qobject_cast<QComboBox*>(sender());

    // 查找 QComboBox 所在行(此法古板，或有更好方法)
    int nCurRow = 0;
    for (; nCurRow < m_pGridTable->rowCount(); ++nCurRow)
    {
        QComboBox* pComboBox = (QComboBox*)m_pGridTable->cellWidget(nCurRow , m_nMoveCurvesCol);
        if (pComboBox == pSender)
        {
            break;
        }
    }

    QTableWidgetItem* pItem = m_pGridTable->item(nCurRow, 0);
    int nActionBlockID = pItem->data(ActionBlockDataID).toInt();
    CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
    if (pGroupManager)
    {
        CActionBlockData* pBlockData = pGroupManager->FindBlockData(nActionBlockID);
        if (pBlockData)
        {
            // 更新最新的数据
            char* pData = NULL;
            int nLen = 0;
            pBlockData->GetExtBlockData(&pData, nLen);
            if (pData && nLen)
            {
                int nMotorCount = m_pGridTable->columnCount() - 3;
                memcpy(pData + 4 + nMotorCount * sizeof(MOTOR_DATA) , &nCurveModel , sizeof(int));

                if (m_pEvent)
                {
                    m_pEvent->ILayerOptionHandler_SetModifiedState(true);
                }
                pBlockData->SetExtBlockData(nLen, pData);
                delete [] pData;
            }
        }
    }
}
