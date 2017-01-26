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
    ui->lblBorderColor->setColor(figcfg->getBorderColor());
    ui->spbDotSize->setValue((int)floor(figcfg->getDotSize()));

    ui->cmbStartArrow->setCurrentText(finFigureArrow::getTypeName(figcfg->getStartArrowType()));
    ui->spbStartArwSize->setValue((int)floor(figcfg->getStartArrowSize()));
    ui->spbStartArwDeg->setValue(_rad_to_deg(figcfg->getStartArrowRadian()));

    ui->cmbEndArrow->setCurrentText(finFigureArrow::getTypeName(figcfg->getEndArrowType()));
    ui->spbEndArwSize->setValue((int)floor(figcfg->getEndArrowSize()));
    ui->spbEndArwDeg->setValue(_rad_to_deg(figcfg->getEndArrowRadian()));

    ui->lblFillingColor->setColor(figcfg->getFillColor());

    ui->fcbFontName->setCurrentFont(figcfg->getFont());
    ui->dsbFontSize->setValue(figcfg->getFontPointSize());
    ui->ckbFontBold->setChecked(figcfg->getFontBold());
    ui->ckbFontItalic->setChecked(figcfg->getFontItalic());
    ui->lblFontColor->setColor(figcfg->getFontColor());
}

void finUiFigConfigDlg::applyToFigureConfig(finFigureConfig *figcfg) const
{
    figcfg->setBorderColor(ui->lblBorderColor->color());
    figcfg->setDotSize((double)ui->spbDotSize->value());

    figcfg->setStartArrowType(finFigureArrow::parseTypeString(ui->cmbStartArrow->currentText()));
    figcfg->setStartArrowSize((double)ui->spbStartArwSize->value());
    figcfg->setStartArrowRadian(_deg_to_rad(ui->spbStartArwDeg->value()));

    figcfg->setEndArrowType(finFigureArrow::parseTypeString(ui->cmbEndArrow->currentText()));
    figcfg->setEndArrowSize((double)ui->spbEndArwSize->value());
    figcfg->setEndArrowRadian(_deg_to_rad(ui->spbEndArwDeg->value()));

    figcfg->setFillColor(ui->lblFillingColor->color());

    figcfg->setFontName(ui->fcbFontName->font().family());
}
