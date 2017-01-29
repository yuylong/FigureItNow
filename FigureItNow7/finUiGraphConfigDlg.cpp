#include "finUiGraphConfigDlg.h"
#include "ui_finUiGraphConfigDlg.h"

#include <qmath.h>
#include <QPointF>

finUiGraphConfigDlg::finUiGraphConfigDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::finUiGraphConfigDlg)
{
    this->_inFilling = true;
    ui->setupUi(this);
    this->_whRatio = 4.0 / 3.0;
    this->_origXPosRatio = 0.5;
    this->_origYPosRatio = 0.5;
    this->_inFilling = false;
}

finUiGraphConfigDlg::~finUiGraphConfigDlg()
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

void finUiGraphConfigDlg::fillFromGraphConfig(const finGraphConfig *graphcfg)
{
    this->_inFilling = true;
    ui->lblBgColor->setColor(graphcfg->getBackgroundColor());

    double pw = graphcfg->getPanelPixelWidth();
    double ph = graphcfg->getPanelPixelHeight();
    ui->spbPanelWidth->setValue((int)floor(pw));
    ui->spbPanelHeight->setValue((int)floor(ph));
    this->_whRatio = pw / ph;

    QPointF origpt = graphcfg->getOriginPixelPoint();
    ui->spbOriginPtX->setValue((int)floor(origpt.x()));
    ui->spbOriginPtY->setValue((int)floor(origpt.y()));
    this->_origXPosRatio = origpt.x() / pw;
    this->_origYPosRatio = origpt.y() / ph;

    ui->dsbAxisUnitSize->setValue(graphcfg->getAxisUnitPixelSize());
    ui->spbAxisZDeg->setValue(_rad_to_deg(graphcfg->getAxisRadZ()));
    ui->dsbAxisZRatio->setValue(graphcfg->getAxisScaleZ());

    this->_inFilling = false;
}

void finUiGraphConfigDlg::applyToGraphConfig(finGraphConfig *graphcfg) const
{

}

void finUiGraphConfigDlg::on_spbPanelWidth_valueChanged(int value)
{
    if ( this->_inFilling )
        return;

    this->_inFilling = true;
    double pw = (double)value;
    double ph = 0.0;
    if ( ui->ckbPanelSizeRatio->isChecked() ) {
        ph = pw / this->_whRatio;
        ui->spbPanelHeight->setValue((int)floor(ph));
    }
    if ( ui->ckbOriginPtRatio->isChecked() ) {
        ui->spbOriginPtX->setValue((int)floor(pw * this->_origXPosRatio));
        if ( ui->ckbPanelSizeRatio->isChecked() )
            ui->spbOriginPtY->setValue((int)floor(ph * this->_origYPosRatio));
    }
    this->_inFilling = false;
}

void finUiGraphConfigDlg::on_spbPanelHeight_valueChanged(int value)
{
    if ( this->_inFilling )
        return;

    this->_inFilling = true;
    double ph = (double)value;
    double pw = 0.0;
    if ( ui->ckbPanelSizeRatio->isChecked() ) {
        pw = ph * this->_whRatio;
        ui->spbPanelWidth->setValue((int)floor(pw));
    }
    if ( ui->ckbOriginPtRatio->isChecked() ) {
        ui->spbOriginPtY->setValue((int)floor(ph * this->_origYPosRatio));
        if ( ui->ckbPanelSizeRatio->isChecked() )
            ui->spbOriginPtX->setValue((int)floor(pw * this->_origXPosRatio));
    }
    this->_inFilling = false;
}

void finUiGraphConfigDlg::on_ckbPanelSizeRatio_stateChanged(int state)
{
    if ( state != Qt::Checked )
        return;

    double pw = (double)ui->spbPanelWidth->value();
    double ph = (double)ui->spbPanelHeight->value();
    this->_whRatio = pw / ph;
}

void finUiGraphConfigDlg::on_ckbOriginPtRatio_stateChanged(int state)
{
    if ( state != Qt::Checked )
        return;

    double pw = (double)ui->spbPanelWidth->value();
    double ph = (double)ui->spbPanelHeight->value();
    this->_origXPosRatio = ui->spbOriginPtX->value() / pw;
    this->_origYPosRatio = ui->spbOriginPtY->value() / ph;
}
