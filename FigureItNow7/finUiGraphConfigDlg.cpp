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

void finUiGraphConfigDlg::on_spbPanelWidth_valueChanged(int arg1)
{
    if ( this->_inFilling )
        return;

    this->_inFilling = true;
    if ( ui->ckbPanelSizeRatio->isChecked() )
        ui->spbPanelHeight->setValue((int)floor(arg1 / this->_whRatio));
    this->_inFilling = false;
}

void finUiGraphConfigDlg::on_spbPanelHeight_valueChanged(int arg1)
{
    if ( this->_inFilling )
        return;

    this->_inFilling = true;
    if ( ui->ckbPanelSizeRatio->isChecked() )
        ui->spbPanelWidth->setValue((int)floor(arg1 * this->_whRatio));
    this->_inFilling = false;
}

void finUiGraphConfigDlg::on_ckbPanelSizeRatio_stateChanged(int arg1)
{
    if ( arg1 != Qt::Checked )
        return;

    double pw = (double)ui->spbPanelWidth->value();
    double ph = (double)ui->spbPanelHeight->value();
    this->_whRatio = pw / ph;
}
