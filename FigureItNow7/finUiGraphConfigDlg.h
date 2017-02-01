#ifndef FINUIGRAPHCONFIGDLG_H
#define FINUIGRAPHCONFIGDLG_H

#include <QDialog>

#include "finGraphConfig.h"

namespace Ui {
class finUiGraphConfigDlg;
}

class finUiGraphConfigDlg : public QDialog
{
    Q_OBJECT

protected:
    bool _inFilling;
    double _whRatio;
    double _origXPosRatio, _origYPosRatio;

public:
    explicit finUiGraphConfigDlg(QWidget *parent = 0);
    ~finUiGraphConfigDlg();

    void fillFromGraphConfig(const finGraphConfig *graphcfg);
    void applyToGraphConfig(finGraphConfig *graphcfg) const;

protected:
    void resetAllTransArgs();
    void fillRectTransArgs(const finGraphTransRect *recttrans);
    void fillAffineTransArgs(const finGraphTransAffine *affinetrans);

    void syncTransStackView(int idx);
    void syncTransAffineActionArg(int idx);
    void syncTransAffineListMoveButton();

private slots:
    void on_spbPanelWidth_valueChanged(int value);
    void on_spbPanelHeight_valueChanged(int value);
    void on_ckbPanelSizeRatio_stateChanged(int state);

    void on_ckbOriginPtRatio_stateChanged(int state);
    void on_spbOriginPtX_valueChanged(int value);
    void on_spbOriginPtY_valueChanged(int value);

    void on_cmbTransformType_currentIndexChanged(int index);
    void on_cmbTransAffineNewAct_currentIndexChanged(int index);

    void on_pbTransAffineReset_clicked();

    void on_pbTransAffineAppendAct_clicked();

    void on_pbTransAffineRemove_clicked();

    void on_pbTransAffineMoveUp_clicked();

    void on_pbTransAffineMoveDown_clicked();

    void on_tbwTransAffineActList_itemSelectionChanged();

private:
    Ui::finUiGraphConfigDlg *ui;
};

#endif // FINUIGRAPHCONFIGDLG_H
