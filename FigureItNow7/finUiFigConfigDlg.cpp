#include "finUiFigConfigDlg.h"
#include "ui_finUiFigConfigDlg.h"

#include <qmath.h>

#include "finFigureArrow.h"


finUiFigConfigDlg::finUiFigConfigDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::finUiFigConfigDlg)
{
    ui->setupUi(this);
    finFigureArrow::fillTypesInComboBox(ui->cmbStartArrow);
    finFigureArrow::fillTypesInComboBox(ui->cmbEndArrow);
}

finUiFigConfigDlg::~finUiFigConfigDlg()
{
    delete ui;
}

void finUiFigConfigDlg::fillFromFigureConfig(finFigureConfig *figcfg)
{
    //ui->lblBorderColor
    ui->spbDotSize->setValue((int)floor(figcfg->getDotSize()));

}

void finUiFigConfigDlg::applyToFigureConfig(finFigureConfig *figcfg) const
{

}
