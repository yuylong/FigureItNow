#ifndef FINUIFIGCONFIGDLG_H
#define FINUIFIGCONFIGDLG_H

#include <QColor>
#include <QDialog>
#include <QLabel>

#include "finFigureConfig.h"


namespace Ui {
class finUiFigConfigDlg;
}

class finUiFigConfigDlg : public QDialog
{
    Q_OBJECT

protected:
    QColor _borderColor;
    QColor _fillingColor;
    QColor _textColor;

public:
    explicit finUiFigConfigDlg(QWidget *parent = 0);
    ~finUiFigConfigDlg();

    void fillFromFigureConfig(finFigureConfig *figcfg);
    void applyToFigureConfig(finFigureConfig *figcfg) const;

private:
    Ui::finUiFigConfigDlg *ui;

    void setupLableBackgroudColor(QLabel *label, const QColor &color);
};

#endif // FINUIFIGCONFIGDLG_H
