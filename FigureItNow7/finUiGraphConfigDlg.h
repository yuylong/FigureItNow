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

private slots:
    void on_spbPanelWidth_valueChanged(int value);
    void on_spbPanelHeight_valueChanged(int value);
    void on_ckbPanelSizeRatio_stateChanged(int state);

    void on_ckbOriginPtRatio_stateChanged(int state);
    void on_spbOriginPtX_valueChanged(int value);
    void on_spbOriginPtY_valueChanged(int value);

private:
    Ui::finUiGraphConfigDlg *ui;
};

#endif // FINUIGRAPHCONFIGDLG_H
