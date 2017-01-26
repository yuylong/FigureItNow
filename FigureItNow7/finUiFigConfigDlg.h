#ifndef FINUIFIGCONFIGDLG_H
#define FINUIFIGCONFIGDLG_H

#include <QDialog>

#include "finFigureConfig.h"


namespace Ui {
class finUiFigConfigDlg;
}

class finUiFigConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit finUiFigConfigDlg(QWidget *parent = 0);
    ~finUiFigConfigDlg();

    void fillFromFigureConfig(finFigureConfig *figcfg);
    void applyToFigureConfig(finFigureConfig *figcfg) const;

private:
    Ui::finUiFigConfigDlg *ui;
};

#endif // FINUIFIGCONFIGDLG_H
