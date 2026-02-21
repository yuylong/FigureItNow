/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#include "finGraphTrans.h"


QString finGraphTrans::getTransformTypeName(finGraphTransType type)
{
    switch ( type ) {
      case finGraphTrans::TP_RECT:
        return QString("rect");
        break;

      case finGraphTrans::TP_AFFINE:
        return QString("affine");
        break;

      default:
        return QString("none");
        break;
    }
}

finGraphTransType finGraphTrans::parseTransformType(const QString &name)
{
    if ( QString::compare(name, QString("none"), Qt::CaseInsensitive) == 0 )
        return finGraphTrans::TP_NONE;
    else if ( QString::compare(name, QString("rect"), Qt::CaseInsensitive) == 0 )
        return finGraphTrans::TP_RECT;
    else if ( QString::compare(name, QString("affine"), Qt::CaseInsensitive) == 0 )
        return finGraphTrans::TP_AFFINE;
    else
        return finGraphTrans::TP_NONE;
}

void finGraphTrans::fillTypesInComboBox(QComboBox *cmbox)
{
    if ( cmbox == nullptr )
        finThrow(finErrorKits::EC_NULL_POINTER, "ComboBox pointer is null");

    cmbox->addItem(QString("None"), QVariant(QString("none")));
    cmbox->addItem(QString("Rectangle"), QVariant(QString("rect")));
    cmbox->addItem(QString("Affine"), QVariant(QString("affine")));
}

int finGraphTrans::getComboBoxIndexForType(QComboBox *cmbox, finGraphTransType type)
{
    if ( cmbox == nullptr )
        return -1;

    QString typestr = finGraphTrans::getTransformTypeName(type);
    for ( int idx = 0; idx < cmbox->count(); idx++ ) {
        QVariant var = cmbox->itemData(idx);
        if ( QString::compare(var.toString(), typestr) == 0 )
            return idx;
    }
    return -1;
}

void finGraphTrans::setComboBoxCurrentItemToType(QComboBox *cmbox, finGraphTrans::Type type)
{
    int idx = finGraphTrans::getComboBoxIndexForType(cmbox, type);
    if ( idx < 0 )
        finThrow(finErrorKits::EC_NOT_FOUND, "Type not found in combo box");

    cmbox->setCurrentIndex(idx);
}

finGraphTrans::finGraphTrans()
{
    this->_type = finGraphTrans::TP_NONE;
}

finGraphTrans::~finGraphTrans()
{
    return;
}

void finGraphTrans::cloneTransform(const finGraphTrans *trans)
{
    if ( trans == nullptr || trans->getTransformType() != this->_type )
        finThrow(finErrorKits::EC_INVALID_PARAM, "Cannot clone transform with different type");
}

finGraphTransType finGraphTrans::getTransformType() const
{
    return this->_type;
}

bool finGraphTrans::isLinear() const
{
    return true;
}

QPointF finGraphTrans::transPoint(const QPointF &ptr)
{
    return ptr;
}

QPointF finGraphTrans::arcTransPoint(const QPointF &ptr)
{
    return ptr;
}

finGraphTransRect::finGraphTransRect()
{
    this->_type = finGraphTrans::TP_RECT;
    this->_axisZoomX = 1.0;
    this->_axisZoomY = 1.0;
}

finGraphTransRect::~finGraphTransRect()
{
    return;
}

void finGraphTransRect::cloneTransform(const finGraphTrans *trans)
{
    finGraphTrans::cloneTransform(trans);

    const finGraphTransRect *srctrans = (const finGraphTransRect *)trans;
    this->_axisZoomX = srctrans->_axisZoomX;
    this->_axisZoomY = srctrans->_axisZoomY;
}

bool finGraphTransRect::isLinear() const
{
    return true;
}

double finGraphTransRect::getAxisZoomX() const
{
    return this->_axisZoomX;
}

double finGraphTransRect::getAxisZoomY() const
{
    return this->_axisZoomY;
}

void finGraphTransRect::setAxisZoomX(double zoomx)
{
    if ( zoomx <= 0.0 )
        finThrow(finErrorKits::EC_INVALID_PARAM, "Axis zoom X must be positive");

    this->_axisZoomX = zoomx;
}

void finGraphTransRect::setAxisZoomY(double zoomy)
{
    if ( zoomy <= 0.0 )
        finThrow(finErrorKits::EC_INVALID_PARAM, "Axis zoom Y must be positive");

    this->_axisZoomY = zoomy;
}

QPointF finGraphTransRect::transPoint(const QPointF &ptr)
{
    QPointF resptr;
    resptr.setX(ptr.x() * this->_axisZoomX);
    resptr.setY(ptr.y() * this->_axisZoomY);
    return resptr;
}

QPointF finGraphTransRect::arcTransPoint(const QPointF &ptr)
{
    QPointF resptr;
    resptr.setX(ptr.x() / this->_axisZoomX);
    resptr.setY(ptr.y() / this->_axisZoomY);
    return resptr;
}

QString finGraphTransAffine::getAffineTransActionName(finGraphTransAffine::ActionType type)
{
    switch ( type ) {
      case finGraphTransAffine::AT_ROTATE:
        return QString("rotate");
        break;

      case finGraphTransAffine::AT_SCALE:
        return QString("scale");
        break;

      case finGraphTransAffine::AT_TRANSLATE:
        return QString("translate");
        break;

      default:
        return QString("none");
        break;
    }
}

finGraphTransAffine::ActionType finGraphTransAffine::parseAffineTransAction(const QString &name)
{
    if ( QString::compare(name, QString("rotate"), Qt::CaseInsensitive) == 0 ) {
        return finGraphTransAffine::AT_ROTATE;
    } else if ( QString::compare(name, QString("scale"), Qt::CaseInsensitive) == 0 ) {
        return finGraphTransAffine::AT_SCALE;
    } else if ( QString::compare(name, QString("translate"), Qt::CaseInsensitive) == 0 ) {
        return finGraphTransAffine::AT_TRANSLATE;
    }
    // We have no dummy value here, and just return a safest one.
    return finGraphTransAffine::AT_DUMMY;
}

void finGraphTransAffine::fillAffineTransActionsInComboBox(QComboBox *cmbox)
{
    if ( cmbox == nullptr )
        finThrow(finErrorKits::EC_NULL_POINTER, "ComboBox pointer is null");

    cmbox->addItem(QString("Rotate"), QVariant(QString("rotate")));
    cmbox->addItem(QString("Scale"), QVariant(QString("scale")));
    cmbox->addItem(QString("Translate"), QVariant(QString("translate")));
}

int finGraphTransAffine::getAffineTransActionArgCnt(finGraphTransAffine::ActionType type)
{
    switch ( type ) {
      case finGraphTransAffine::AT_ROTATE:
        return 1;
        break;

      case finGraphTransAffine::AT_SCALE:
      case finGraphTransAffine::AT_TRANSLATE:
        return 2;
        break;

      default:
        return 0;
        break;
    }
}

finGraphTransAffine::ActArgType
finGraphTransAffine::getAffineTransActionArgType(finGraphTransAffine::ActionType type)
{
    switch ( type ) {
      case finGraphTransAffine::AT_ROTATE:
        return finGraphTransAffine::AAT_RADIAN;
        break;

      case finGraphTransAffine::AT_SCALE:
      case finGraphTransAffine::AT_TRANSLATE:
        return finGraphTransAffine::AAT_XY;
        break;

      default:
        return finGraphTransAffine::AAT_NONE;
        break;
    }
}

finGraphTransAffine::finGraphTransAffine()
    : _matrix(), _invMatrix()
{
    this->_type = finGraphTrans::TP_AFFINE;
}

finGraphTransAffine::~finGraphTransAffine()
{
    return;
}

void finGraphTransAffine::cloneTransform(const finGraphTrans *trans)
{
    finGraphTrans::cloneTransform(trans);

    const finGraphTransAffine *srctrans = (const finGraphTransAffine *)trans;
    this->_actList.clear();
    this->_actList = srctrans->_actList;
    this->_matrix = srctrans->_matrix;
    this->_invMatrix = srctrans->_invMatrix;
}

bool finGraphTransAffine::isLinear() const
{
    return true;
}

QTransform finGraphTransAffine::getTransformMatrix() const
{
    return this->_matrix;
}

QTransform finGraphTransAffine::getInvertedTransformMatrix() const
{
    return this->_invMatrix;
}

int finGraphTransAffine::getActionCount() const
{
    return this->_actList.count();
}

finGraphTransAffine::Action finGraphTransAffine::getActionAt(int idx) const
{
    static const finGraphTransAffine::Action defact = { finGraphTransAffine::AT_ROTATE, 0.0, 0.0 };

    if ( idx < 0 || idx >= this->_actList.count() )
        return defact;
    else
        return this->_actList.at(idx);
}

void finGraphTransAffine::reset()
{
    this->_actList.clear();
    this->_matrix.reset();
    this->_invMatrix.reset();
}

void finGraphTransAffine::calcInvertedMatrix()
{
    if ( !this->_matrix.isInvertible() ) {
        this->_invMatrix.reset();
        return;
    }

    this->_invMatrix = this->_matrix.inverted();
}

void finGraphTransAffine::appendRotate(double rad)
{
    QTransform subtrans;
    subtrans.rotateRadians(rad);
    this->_matrix *= subtrans;
    this->calcInvertedMatrix();

    finGraphTransAffine::Action act;
    act._type = finGraphTransAffine::AT_ROTATE;
    act._arg1 = act._arg2 = rad;
    this->_actList.append(act);
}

void finGraphTransAffine::appendScale(double sx, double sy)
{
    QTransform subtrans;
    subtrans.scale(sx, sy);
    this->_matrix *= subtrans;
    this->calcInvertedMatrix();

    finGraphTransAffine::Action act;
    act._type = finGraphTransAffine::AT_SCALE;
    act._arg1 = sx;
    act._arg2 = sy;
    this->_actList.append(act);
}

void finGraphTransAffine::appendTranslate(double tx, double ty)
{
    QTransform subtrans;
    subtrans.translate(tx, ty);
    this->_matrix *= subtrans;
    this->calcInvertedMatrix();

    finGraphTransAffine::Action act;
    act._type = finGraphTransAffine::AT_TRANSLATE;
    act._arg1 = tx;
    act._arg2 = ty;
    this->_actList.append(act);
}

QPointF finGraphTransAffine::transPoint(const QPointF &ptr)
{
    return this->_matrix.map(ptr);
}

QPointF finGraphTransAffine::arcTransPoint(const QPointF &ptr)
{
    return this->_invMatrix.map(ptr);
}
