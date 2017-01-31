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

static inline double _rad_to_lf_deg(double rad)
{
    return rad * 180.0 / M_PI;
}

static inline int _rad_to_deg(double rad)
{
    return (int)floor(_rad_to_lf_deg(rad));
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

    finGraphTransType transtype = graphcfg->getTransformType();
    finGraphTrans::setComboBoxCurrentItemToType(ui->cmbTransformType, transtype);
    syncTransStackView(ui->cmbTransformType->currentIndex());

    finGraphTrans *trans = graphcfg->getTransform();
    this->resetAllTransArgs();
    if ( transtype == finGraphTrans::FIN_GT_TYPE_RECT )
        this->fillRectTransArgs((finGraphTransRect *)trans);
    else if ( transtype == finGraphTrans::FIN_GT_TYPE_AFFINE )
        this->fillAffineTransArgs((finGraphTransAffine *)trans);

    this->_inFilling = false;
}

void finUiGraphConfigDlg::resetAllTransArgs()
{
    // Rect Transformation
    ui->dsbTransRectZoomX->setValue(1.0);
    ui->dsbTransRectZoomY->setValue(1.0);

    // Affine Transformation
    ui->tbwTransAffineActList->clearContents();
    ui->tbwTransAffineActList->setRowCount(0);
}

void finUiGraphConfigDlg::fillRectTransArgs(const finGraphTransRect *recttrans)
{
    ui->dsbTransRectZoomX->setValue(recttrans->getAxisZoomX());
    ui->dsbTransRectZoomY->setValue(recttrans->getAxisZoomY());
}

void finUiGraphConfigDlg::fillAffineTransArgs(const finGraphTransAffine *affinetrans)
{
    int rowcnt = affinetrans->getActionCount();
    ui->tbwTransAffineActList->clearContents();
    ui->tbwTransAffineActList->setRowCount(rowcnt);

    for ( int i = 0; i < rowcnt; i++ ) {
        finGraphTransAffine::Action curact = affinetrans->getActionAt(i);
        QTableWidgetItem *tblitem;

        tblitem = new QTableWidgetItem(finGraphTransAffine::getAffineTransActionName(curact._type));
        tblitem->setTextAlignment(Qt::AlignCenter);
        ui->tbwTransAffineActList->setItem(i, 0, tblitem);

        double arg1 = curact._arg1;
        if ( curact._type == finGraphTransAffine::FIN_GTA_TYPE_ROTATE )
            arg1 = _rad_to_lf_deg(arg1);
        tblitem = new QTableWidgetItem(QString::number(arg1));
        tblitem->setTextAlignment(Qt::AlignCenter);
        ui->tbwTransAffineActList->setItem(i, 1, tblitem);

        if ( finGraphTransAffine::getAffineTransActionArgCnt(curact._type) >= 2 ) {
            tblitem = new QTableWidgetItem(QString::number(curact._arg2));
            tblitem->setTextAlignment(Qt::AlignCenter);
            ui->tbwTransAffineActList->setItem(i, 2, tblitem);
        }
    }
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
    finGraphTransAffine::ActionType acttype = finGraphTransAffine::FIN_GTA_TYPE_DUMMY;

    if ( ui->cmbTransAffineNewAct->count() > 0 ) {
        QString actstr = ui->cmbTransAffineNewAct->itemData(idx).toString();
        acttype = finGraphTransAffine::parseAffineTransAction(actstr);
    }

    switch ( acttype ) {
      case finGraphTransAffine::FIN_GTA_TYPE_ROTATE:
        ui->lblTransAffineArgXTitle->setEnabled(true);
        ui->dsbTransAffineArgX->setEnabled(true);
        ui->dsbTransAffineArgX->setRange(-179.0, 180.0);
        ui->dsbTransAffineArgX->setSingleStep(5.0);
        ui->dsbTransAffineArgX->setDecimals(1);
        ui->dsbTransAffineArgX->setValue(0.0);

        ui->lblTransAffineArgYTitle->setEnabled(false);
        ui->dsbTransAffineArgY->setEnabled(false);
        break;

      case finGraphTransAffine::FIN_GTA_TYPE_SCALE:
      case finGraphTransAffine::FIN_GTA_TYPE_TRANSLATE:
        ui->lblTransAffineArgXTitle->setEnabled(true);
        ui->dsbTransAffineArgX->setEnabled(true);
        ui->dsbTransAffineArgX->setRange(-65535.0, 65535.0);
        ui->dsbTransAffineArgX->setSingleStep(1.0);
        ui->dsbTransAffineArgX->setDecimals(1);
        if ( acttype == finGraphTransAffine::FIN_GTA_TYPE_SCALE )
            ui->dsbTransAffineArgX->setValue(1.0);
        else
            ui->dsbTransAffineArgX->setValue(0.0);

        ui->lblTransAffineArgYTitle->setEnabled(true);
        ui->dsbTransAffineArgY->setEnabled(true);
        ui->dsbTransAffineArgY->setRange(-65535.0, 65535.0);
        ui->dsbTransAffineArgY->setSingleStep(1.0);
        ui->dsbTransAffineArgY->setDecimals(1);
        if ( acttype == finGraphTransAffine::FIN_GTA_TYPE_SCALE )
            ui->dsbTransAffineArgY->setValue(1.0);
        else
            ui->dsbTransAffineArgY->setValue(0.0);
        break;

      default:
        ui->lblTransAffineArgXTitle->setEnabled(false);
        ui->dsbTransAffineArgX->setEnabled(false);
        ui->lblTransAffineArgYTitle->setEnabled(false);
        ui->dsbTransAffineArgY->setEnabled(false);
        break;
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

void finUiGraphConfigDlg::on_pbTransAffineReset_clicked()
{
    ui->tbwTransAffineActList->clearContents();
    ui->tbwTransAffineActList->setRowCount(0);
}

void finUiGraphConfigDlg::on_pbTransAffineAppendAct_clicked()
{
    QTableWidgetItem *tblitem;
    int newrowidx = ui->tbwTransAffineActList->rowCount();
    ui->tbwTransAffineActList->insertRow(newrowidx);

    tblitem = new QTableWidgetItem(ui->cmbTransAffineNewAct->currentData().toString());
    tblitem->setTextAlignment(Qt::AlignCenter);
    ui->tbwTransAffineActList->setItem(newrowidx, 0, tblitem);

    if ( ui->dsbTransAffineArgX->isEnabled() ) {
        tblitem = new QTableWidgetItem(QString::number(ui->dsbTransAffineArgX->value()));
        tblitem->setTextAlignment(Qt::AlignCenter);
        ui->tbwTransAffineActList->setItem(newrowidx, 1, tblitem);
    }

    if ( ui->dsbTransAffineArgY->isEnabled() ) {
        tblitem = new QTableWidgetItem(QString::number(ui->dsbTransAffineArgY->value()));
        tblitem->setTextAlignment(Qt::AlignCenter);
        ui->tbwTransAffineActList->setItem(newrowidx, 2, tblitem);
    }
}
