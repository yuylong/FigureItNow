/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#ifndef FINUIFIGCONFIGDLG_H
#define FINUIFIGCONFIGDLG_H

#include <QColor>
#include <QDialog>
#include <QLabel>

#include "finFigureConfig.h"
#include "finUiColorLabel.h"


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
};

#endif // FINUIFIGCONFIGDLG_H
