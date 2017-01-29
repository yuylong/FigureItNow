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

private:
    Ui::finUiGraphConfigDlg *ui;
};

#endif // FINUIGRAPHCONFIGDLG_H
