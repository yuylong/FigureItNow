/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

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
    return static_cast<int>(floor(_rad_to_lf_deg(rad)));
}

static inline double _deg_to_rad(int deg)
{
    return static_cast<double>(deg) * M_PI / 180.0;
}

void finUiGraphConfigDlg::fillFromGraphConfig(const finGraphConfig *graphcfg)
{
    this->_inFilling = true;
    ui->lblBgColor->setColor(graphcfg->getBackgroundColor());

    double pw = graphcfg->getPanelPixelWidth();
    double ph = graphcfg->getPanelPixelHeight();
    ui->spbPanelWidth->setValue(static_cast<int>(floor(pw)));
    ui->spbPanelHeight->setValue(static_cast<int>(floor(ph)));
    this->_whRatio = pw / ph;

    QPointF origpt = graphcfg->getOriginPixelPoint();
    ui->spbOriginPtX->setValue(static_cast<int>(floor(origpt.x())));
    ui->spbOriginPtY->setValue(static_cast<int>(floor(origpt.y())));
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
    if ( transtype == finGraphTrans::TP_RECT )
        this->fillRectTransArgs(static_cast<finGraphTransRect *>(trans));
    else if ( transtype == finGraphTrans::TP_AFFINE )
        this->fillAffineTransArgs(static_cast<finGraphTransAffine *>(trans));

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
    this->syncTransAffineListMoveButton();
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
        if ( curact._type == finGraphTransAffine::AT_ROTATE )
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
    this->syncTransAffineListMoveButton();
}

void finUiGraphConfigDlg::applyToGraphConfig(finGraphConfig *graphcfg) const
{
    if ( graphcfg == nullptr )
        return /*finErrorKits::EC_NULL_POINTER*/;

    graphcfg->setBackgroundColor(ui->lblBgColor->color());
    graphcfg->setPanelPixelWidth(static_cast<double>(ui->spbPanelWidth->value()));
    graphcfg->setPanelPixelHeight(static_cast<double>(ui->spbPanelHeight->value()));
    graphcfg->setOriginPixelPointX(static_cast<double>(ui->spbOriginPtX->value()));
    graphcfg->setOriginPixelPointY(static_cast<double>(ui->spbOriginPtY->value()));

    graphcfg->setAxisUnitPixelSize(static_cast<double>(ui->dsbAxisUnitSize->value()));
    graphcfg->setAxisRadZ(_deg_to_rad(ui->spbAxisZDeg->value()));
    graphcfg->setAxisScaleZ(ui->dsbAxisZRatio->value());

    finGraphTransType transtype = finGraphTrans::parseTransformType(
                ui->cmbTransformType->currentData().toString());
    graphcfg->setTransformType(transtype);

    finGraphTrans *trans = graphcfg->getTransform();
    if ( transtype == finGraphTrans::TP_RECT )
        this->applyRectTransArgs(static_cast<finGraphTransRect *>(trans));
    else if ( transtype == finGraphTrans::TP_AFFINE )
        this->applyAffineTransArgs(static_cast<finGraphTransAffine *>(trans));
}

void finUiGraphConfigDlg::applyRectTransArgs(finGraphTransRect *recttrans) const
{
    if ( recttrans == nullptr )
        return /*finErrorKits::EC_NULL_POINTER*/;

    double zoomx = ui->dsbTransRectZoomX->value();
    double zoomy = ui->dsbTransRectZoomY->value();

    if ( zoomx > -1.0e-8 && zoomx < 1.0e-8 )
        recttrans->setAxisZoomX(1.0);
    else
        recttrans->setAxisZoomX(ui->dsbTransRectZoomX->value());

    if ( zoomy > -1.0e-8 && zoomy < 1.0e-8 )
        recttrans->setAxisZoomY(1.0);
    else
        recttrans->setAxisZoomY(ui->dsbTransRectZoomY->value());
}

void finUiGraphConfigDlg::applyAffineTransArgs(finGraphTransAffine *affinetrans) const
{
    if ( affinetrans == nullptr )
        return /*finErrorKits::EC_NULL_POINTER*/;

    affinetrans->reset();
    for ( int rowidx = 0; rowidx < ui->tbwTransAffineActList->rowCount(); rowidx++ ) {
        QTableWidgetItem *tblitem;
        finGraphTransAffine::ActionType acttype = finGraphTransAffine::AT_DUMMY;
        double arg1 = 0.0, arg2 = 0.0;

        tblitem = ui->tbwTransAffineActList->item(rowidx, 0);
        if ( tblitem != nullptr )
            acttype = finGraphTransAffine::parseAffineTransAction(tblitem->text());

        tblitem = ui->tbwTransAffineActList->item(rowidx, 1);
        if ( tblitem != nullptr )
            arg1 = tblitem->text().toDouble();

        tblitem = ui->tbwTransAffineActList->item(rowidx, 2);
        if ( tblitem != nullptr )
            arg2 = tblitem->text().toDouble();

        switch ( acttype ) {
          case finGraphTransAffine::AT_ROTATE:
            affinetrans->appendRotate(_deg_to_rad(static_cast<int>(floor(arg1))));
            break;

          case finGraphTransAffine::AT_SCALE:
            if ( arg1 > -1.0e-8 && arg1 < 1.0e-8 )
                arg1 = 1.0;
            if ( arg2 > -1.0e-8 && arg2 < 1.0e-8 )
                arg2 = 1.0;
            affinetrans->appendScale(arg1, arg2);
            break;

          case finGraphTransAffine::AT_TRANSLATE:
            affinetrans->appendTranslate(arg1, arg2);
            break;

          default:
            break;
        }
    }
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
    finGraphTransAffine::ActionType acttype = finGraphTransAffine::AT_DUMMY;

    if ( ui->cmbTransAffineNewAct->count() > 0 ) {
        QString actstr = ui->cmbTransAffineNewAct->itemData(idx).toString();
        acttype = finGraphTransAffine::parseAffineTransAction(actstr);
    }

    switch ( acttype ) {
      case finGraphTransAffine::AT_ROTATE:
        ui->lblTransAffineArgXTitle->setEnabled(true);
        ui->dsbTransAffineArgX->setEnabled(true);
        ui->dsbTransAffineArgX->setRange(-179.0, 180.0);
        ui->dsbTransAffineArgX->setSingleStep(5.0);
        ui->dsbTransAffineArgX->setDecimals(1);
        ui->dsbTransAffineArgX->setValue(0.0);

        ui->lblTransAffineArgYTitle->setEnabled(false);
        ui->dsbTransAffineArgY->setEnabled(false);
        break;

      case finGraphTransAffine::AT_SCALE:
      case finGraphTransAffine::AT_TRANSLATE:
        ui->lblTransAffineArgXTitle->setEnabled(true);
        ui->dsbTransAffineArgX->setEnabled(true);
        ui->dsbTransAffineArgX->setRange(-65535.0, 65535.0);
        ui->dsbTransAffineArgX->setSingleStep(1.0);
        ui->dsbTransAffineArgX->setDecimals(1);
        if ( acttype == finGraphTransAffine::AT_SCALE )
            ui->dsbTransAffineArgX->setValue(1.0);
        else
            ui->dsbTransAffineArgX->setValue(0.0);

        ui->lblTransAffineArgYTitle->setEnabled(true);
        ui->dsbTransAffineArgY->setEnabled(true);
        ui->dsbTransAffineArgY->setRange(-65535.0, 65535.0);
        ui->dsbTransAffineArgY->setSingleStep(1.0);
        ui->dsbTransAffineArgY->setDecimals(1);
        if ( acttype == finGraphTransAffine::AT_SCALE )
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
    double pw = static_cast<double>(value);
    double ph = 0.0;
    if ( ui->ckbPanelSizeRatio->isChecked() ) {
        ph = pw / this->_whRatio;
        ui->spbPanelHeight->setValue(static_cast<int>(floor(ph)));
    }
    if ( ui->ckbOriginPtRatio->isChecked() ) {
        ui->spbOriginPtX->setValue(static_cast<int>(floor(pw * this->_origXPosRatio)));
        if ( ui->ckbPanelSizeRatio->isChecked() )
            ui->spbOriginPtY->setValue(static_cast<int>(floor(ph * this->_origYPosRatio)));
    }
    this->_inFilling = false;
}

void finUiGraphConfigDlg::on_spbPanelHeight_valueChanged(int value)
{
    if ( this->_inFilling )
        return;

    this->_inFilling = true;
    double ph = static_cast<double>(value);
    double pw = 0.0;
    if ( ui->ckbPanelSizeRatio->isChecked() ) {
        pw = ph * this->_whRatio;
        ui->spbPanelWidth->setValue(static_cast<int>(floor(pw)));
    }
    if ( ui->ckbOriginPtRatio->isChecked() ) {
        ui->spbOriginPtY->setValue(static_cast<int>(floor(ph * this->_origYPosRatio)));
        if ( ui->ckbPanelSizeRatio->isChecked() )
            ui->spbOriginPtX->setValue(static_cast<int>(floor(pw * this->_origXPosRatio)));
    }
    this->_inFilling = false;
}

void finUiGraphConfigDlg::on_ckbPanelSizeRatio_stateChanged(int state)
{
    if ( state != Qt::Checked )
        return;

    double pw = static_cast<double>(ui->spbPanelWidth->value());
    double ph = static_cast<double>(ui->spbPanelHeight->value());
    this->_whRatio = pw / ph;
}

void finUiGraphConfigDlg::on_ckbOriginPtRatio_stateChanged(int state)
{
    if ( state != Qt::Checked )
        return;

    double pw = static_cast<double>(ui->spbPanelWidth->value());
    double ph = static_cast<double>(ui->spbPanelHeight->value());
    this->_origXPosRatio = ui->spbOriginPtX->value() / pw;
    this->_origYPosRatio = ui->spbOriginPtY->value() / ph;
}

void finUiGraphConfigDlg::on_spbOriginPtX_valueChanged(int value)
{
    if ( this->_inFilling )
        return;

    this->_inFilling = true;
    if ( ui->ckbOriginPtRatio->isChecked() ) {
        double pw = static_cast<double>(ui->spbPanelWidth->value());
        this->_origXPosRatio = static_cast<double>(value) / pw;
    }
    this->_inFilling = false;
}

void finUiGraphConfigDlg::on_spbOriginPtY_valueChanged(int value)
{
    if ( this->_inFilling )
        return;

    this->_inFilling = true;
    if ( ui->ckbOriginPtRatio->isChecked() ) {
        double ph = static_cast<double>(ui->spbPanelHeight->value());
        this->_origYPosRatio = static_cast<double>(value) / ph;
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
    this->syncTransAffineListMoveButton();
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
    this->syncTransAffineListMoveButton();
}

void finUiGraphConfigDlg::on_pbTransAffineRemove_clicked()
{
    QList<QTableWidgetSelectionRange> selrange = ui->tbwTransAffineActList->selectedRanges();
    if ( selrange.count() <= 0 )
        return;

    ui->tbwTransAffineActList->clearSelection();
    for ( int i = selrange.count() - 1; i >= 0; i-- ) {
        const QTableWidgetSelectionRange &currange = selrange.at(i);
        for ( int rowidx = currange.bottomRow(); rowidx >= currange.topRow(); rowidx-- ) {
            ui->tbwTransAffineActList->removeRow(rowidx);
        }
    }
    this->syncTransAffineListMoveButton();
}

void finUiGraphConfigDlg::syncTransAffineListMoveButton()
{
    int rowcnt = ui->tbwTransAffineActList->rowCount();
    if ( rowcnt <= 0 )
        ui->pbTransAffineReset->setEnabled(false);
    else
        ui->pbTransAffineReset->setEnabled(true);


    QList<QTableWidgetSelectionRange> selrange = ui->tbwTransAffineActList->selectedRanges();
    if ( selrange.count() <= 0 ) {
        ui->pbTransAffineRemove->setEnabled(false);
        ui->pbTransAffineMoveUp->setEnabled(false);
        ui->pbTransAffineMoveDown->setEnabled(false);
        return;
    }

    ui->pbTransAffineRemove->setEnabled(true);
    if ( rowcnt < 2 ) {
        ui->pbTransAffineMoveUp->setEnabled(false);
        ui->pbTransAffineMoveDown->setEnabled(false);
        return;
    }

    int currow = selrange.first().topRow();
    if ( currow == 0 ) {
        ui->pbTransAffineMoveUp->setEnabled(false);
        ui->pbTransAffineMoveDown->setEnabled(true);
    } else if ( currow >= ui->tbwTransAffineActList->rowCount() - 1 ) {
        ui->pbTransAffineMoveUp->setEnabled(true);
        ui->pbTransAffineMoveDown->setEnabled(false);
    } else {
        ui->pbTransAffineMoveUp->setEnabled(true);
        ui->pbTransAffineMoveDown->setEnabled(true);
    }
}

void finUiGraphConfigDlg::on_pbTransAffineMoveUp_clicked()
{
    QList<QTableWidgetSelectionRange> selrange = ui->tbwTransAffineActList->selectedRanges();
    if ( selrange.count() <= 0 )
        return;

    int currow = selrange.first().topRow();
    if ( currow < 1 )
        return;

    QTableWidgetItem *curitem, *preitem;
    for ( int colidx = 0; colidx < ui->tbwTransAffineActList->columnCount(); colidx++ ) {
        curitem = ui->tbwTransAffineActList->takeItem(currow, colidx);
        preitem = ui->tbwTransAffineActList->takeItem(currow - 1, colidx);

        ui->tbwTransAffineActList->setItem(currow, colidx, preitem);
        ui->tbwTransAffineActList->setItem(currow - 1, colidx, curitem);
    }
    ui->tbwTransAffineActList->selectRow(currow - 1);
}

void finUiGraphConfigDlg::on_pbTransAffineMoveDown_clicked()
{
    QList<QTableWidgetSelectionRange> selrange = ui->tbwTransAffineActList->selectedRanges();
    if ( selrange.count() <= 0 )
        return;

    int currow = selrange.first().topRow();
    if ( currow >= ui->tbwTransAffineActList->rowCount() - 1 )
        return;

    QTableWidgetItem *curitem, *nxtitem;
    for ( int colidx = 0; colidx < ui->tbwTransAffineActList->columnCount(); colidx++ ) {
        curitem = ui->tbwTransAffineActList->takeItem(currow, colidx);
        nxtitem = ui->tbwTransAffineActList->takeItem(currow + 1, colidx);

        ui->tbwTransAffineActList->setItem(currow, colidx, nxtitem);
        ui->tbwTransAffineActList->setItem(currow + 1, colidx, curitem);
    }
    ui->tbwTransAffineActList->selectRow(currow + 1);
}

void finUiGraphConfigDlg::on_tbwTransAffineActList_itemSelectionChanged()
{
    this->syncTransAffineListMoveButton();
}
