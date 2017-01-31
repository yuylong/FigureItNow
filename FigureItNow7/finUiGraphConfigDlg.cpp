#include "finUiGraphConfigDlg.h"
#include "ui_finUiGraphConfigDlg.h"

#include <qmath.h>
#include <QPointF>

#include "finErrorCode.h"
#include "finGraphTrans.h"

finUiGraphConfigDlg::finUiGraphConfigDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::finUiGraphConfigDlg)
{
    this->_inFilling = true;
    ui->setupUi(this);
    finGraphTrans::fillTypesInComboBox(ui->cmbTransformType);
    finGraphTransAffine::fillAffineTransActionsInComboBox(ui->cmbTransAffineNewAct);
    ui->cmbTransAffineNewAct->setCurrentIndex(0);
    this->syncTransAffineActionArg(0);

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

    finGraphTrans::setComboBoxCurrentItemToType(ui->cmbTransformType, graphcfg->getTransformType());
    syncTransStackView(ui->cmbTransformType->currentIndex());

    this->_inFilling = false;
}

void finUiGraphConfigDlg::applyToGraphConfig(finGraphConfig *graphcfg) const
{

}

void finUiGraphConfigDlg::syncTransStackView(int idx)
{
    if ( ui->cmbTransformType->count() <= 0 )
        return;

    if ( idx < 0 || idx >= ui->cmbTransformType->count() )
        idx = ui->cmbTransformType->currentIndex();

    QString curdat = ui->cmbTransformType->itemData(idx).toString();
    if ( QString::compare(curdat, QString("none")) == 0 )
        ui->stwTransformArgs->setCurrentWidget(ui->stpTransArgNone);
    else if ( QString::compare(curdat, QString("rect")) == 0 )
        ui->stwTransformArgs->setCurrentWidget(ui->stpTransArgRect);
    else if ( QString::compare(curdat, QString("affine")) == 0 )
        ui->stwTransformArgs->setCurrentWidget(ui->stpTransArgAffine);
    return;
}

void finUiGraphConfigDlg::syncTransAffineActionArg(int idx)
{
    int argcnt = 0;

    if ( ui->cmbTransAffineNewAct->count() <= 0 ) {
        argcnt = 0;
    } else {
        QString actstr = ui->cmbTransAffineNewAct->itemData(idx).toString();
        finGraphTransAffine::ActionType acttype = finGraphTransAffine::parseAffineTransAction(actstr);
        argcnt = finGraphTransAffine::getAffineTransActionArgCnt(acttype);
    }

    if ( argcnt <= 0 ) {
        ui->dsbTransAffineArgX->setEnabled(false);
        ui->dsbTransAffineArgY->setEnabled(false);
    } else if ( argcnt == 1 ) {
        ui->dsbTransAffineArgX->setEnabled(true);
        ui->dsbTransAffineArgY->setEnabled(false);
    } else /*if ( argcnt >= 2 )*/ {
        ui->dsbTransAffineArgX->setEnabled(true);
        ui->dsbTransAffineArgY->setEnabled(true);
    }
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

void finUiGraphConfigDlg::on_spbOriginPtX_valueChanged(int value)
{
    if ( this->_inFilling )
        return;

    this->_inFilling = true;
    if ( ui->ckbOriginPtRatio->isChecked() ) {
        double pw = (double)ui->spbPanelWidth->value();
        this->_origXPosRatio = (double)value / pw;
    }
    this->_inFilling = false;
}

void finUiGraphConfigDlg::on_spbOriginPtY_valueChanged(int value)
{
    if ( this->_inFilling )
        return;

    this->_inFilling = true;
    if ( ui->ckbOriginPtRatio->isChecked() ) {
        double ph = (double)ui->spbPanelHeight->value();
        this->_origYPosRatio = (double)value / ph;
    }
    this->_inFilling = false;
}

void finUiGraphConfigDlg::on_cmbTransformType_currentIndexChanged(int index)
{
    this->syncTransStackView(index);
}

void finUiGraphConfigDlg::on_cmbTransAffineNewAct_currentIndexChanged(int index)
{
    this->syncTransAffineActionArg(index);
}
