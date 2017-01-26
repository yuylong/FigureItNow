#include "finUiFigConfigDlg.h"
#include "ui_finUiFigConfigDlg.h"

#include <qmath.h>
#include <QPalette>

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

void finUiFigConfigDlg::setupLableBackgroudColor(QLabel *label, const QColor &color)
{
    QPalette palette = label->palette();

    palette.setColor(QPalette::Active, QPalette::Base, color);
    palette.setColor(QPalette::Disabled, QPalette::Base, color);
    palette.setColor(QPalette::Inactive, QPalette::Base, color);

    label->setPalette(palette);
    label->setBackgroundRole(QPalette::Base);
    label->setAutoFillBackground(true);
}

static inline int _rad_to_deg(double rad)
{
    return (int)floor(rad * 180.0 / M_PI);
}

static inline double _deg_to_rad(int deg)
{
    return (double)deg * M_PI / 180.0;
}

void finUiFigConfigDlg::fillFromFigureConfig(finFigureConfig *figcfg)
{
    this->setupLableBackgroudColor(ui->lblBorderColor, figcfg->getBorderColor());
    ui->spbDotSize->setValue((int)floor(figcfg->getDotSize()));

    ui->cmbStartArrow->setCurrentText(finFigureArrow::getTypeName(figcfg->getStartArrowType()));
    ui->spbStartArwSize->setValue((int)floor(figcfg->getStartArrowSize()));
    ui->spbStartArwDeg->setValue(_rad_to_deg(figcfg->getStartArrowRadian()));

    ui->cmbEndArrow->setCurrentText(finFigureArrow::getTypeName(figcfg->getEndArrowType()));
    ui->spbEndArwSize->setValue((int)floor(figcfg->getEndArrowSize()));
    ui->spbEndArwDeg->setValue(_rad_to_deg(figcfg->getEndArrowRadian()));

    this->setupLableBackgroudColor(ui->lblFillingColor, figcfg->getFillColor());

    ui->fcbFontName->setCurrentFont(figcfg->getFont());
    ui->dsbFontSize->setValue(figcfg->getFontPointSize());
    ui->ckbFontBold->setChecked(figcfg->getFontBold());
    ui->ckbFontItalic->setChecked(figcfg->getFontItalic());
    this->setupLableBackgroudColor(ui->lblFontColor, figcfg->getFontColor());
}

void finUiFigConfigDlg::applyToFigureConfig(finFigureConfig *figcfg) const
{

}
