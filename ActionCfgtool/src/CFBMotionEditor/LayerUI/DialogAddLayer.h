#ifndef DIALOGADDLAYER_H
#define DIALOGADDLAYER_H

#include "motioneditor_global.h"
#include <QDialog>
#include <QCheckBox>

namespace Ui {
class CDialogAddLayer;
}

enum AddLayerItemRoles
{
    LayerType = Qt::UserRole + 1
};

class MOTIONEDITOR_EXPORT CDialogAddLayer : public QDialog
{
    Q_OBJECT

public:
    explicit CDialogAddLayer(QWidget *parent = 0);
    ~CDialogAddLayer();

    
    void InitLayerList(int **pLayerID, int &nLayerCount);

    
    void GetSelectLayer(int** ppIDs, int &nCount);

    
    QString GetLayerNameByType(int nType);

    bool IsLayerExist(int nType);

protected:

    
    virtual void paintEvent(QPaintEvent *);

private slots:
    void on_pbClose_clicked();

    void on_pbCancel_clicked();

    void on_pbOK_clicked();

    void slotCheckState(int);

private:
    Ui::CDialogAddLayer *ui;
    int* m_pIDs;
    int m_nSelectCount;
    QVector<int> m_vUnUseID;
    QMap <QCheckBox*, int> m_mapCheckBox;

    QCheckBox* findCheckBoxByID(int nLayerID);
};

#endif // DIALOGADDLAYER_H
