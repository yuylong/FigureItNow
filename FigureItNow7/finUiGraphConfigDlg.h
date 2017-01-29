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
    void on_spbPanelWidth_valueChanged(int arg1);

    void on_spbPanelHeight_valueChanged(int arg1);

    void on_ckbPanelSizeRatio_stateChanged(int arg1);

private:
    Ui::finUiGraphConfigDlg *ui;
};

#endif // FINUIGRAPHCONFIGDLG_H
