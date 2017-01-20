#include "finGraphTrans.h"

finGraphTrans::finGraphTrans()
{
    this->_type = finGraphTrans::FIN_GT_TYPE_NONE;
}

finErrorCode finGraphTrans::cloneTransform(const finGraphTrans *trans)
{
    if ( trans == NULL || trans->getTransformType() != this->_type )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    return finErrorCodeKits::FIN_EC_SUCCESS;
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
    this->_type = finGraphTrans::FIN_GT_TYPE_RECT;
    this->_axisZoomX = 1.0;
    this->_axisZoomY = 1.0;
}

finErrorCode finGraphTransRect::cloneTransform(const finGraphTrans *trans)
{
    finErrorCode errcode = finGraphTrans::cloneTransform(trans);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    const finGraphTransRect *srctrans = (const finGraphTransRect *)trans;
    this->_axisZoomX = srctrans->_axisZoomX;
    this->_axisZoomY = srctrans->_axisZoomY;
    return finErrorCodeKits::FIN_EC_SUCCESS;
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

finErrorCode finGraphTransRect::setAxisZoomX(double zoomx)
{
    if ( zoomx <= 0.0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_axisZoomX = zoomx;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphTransRect::setAxisZoomY(double zoomy)
{
    if ( zoomy <= 0.0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_axisZoomY = zoomy;
    return finErrorCodeKits::FIN_EC_SUCCESS;
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

finGraphTransAffine::finGraphTransAffine()
    : _matrix(), _invMatrix()
{
    this->_type = finGraphTrans::FIN_GT_TYPE_AFFINE;
}

finErrorCode finGraphTransAffine::cloneTransform(const finGraphTrans *trans)
{
    finErrorCode errcode = finGraphTrans::cloneTransform(trans);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    const finGraphTransAffine *srctrans = (const finGraphTransAffine *)trans;
    this->_actList.clear();
    this->_actList = srctrans->_actList;
    this->_matrix = srctrans->_matrix;
    this->_invMatrix = srctrans->_invMatrix;
    return finErrorCodeKits::FIN_EC_SUCCESS;
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
    static const finGraphTransAffine::Action defact = { finGraphTransAffine::FIN_GTA_TYPE_ROTATE, 0.0, 0.0 };

    if ( idx < 0 || idx >= this->_actList.count() )
        return defact;
    else
        return this->_actList.at(idx);
}

finErrorCode finGraphTransAffine::reset()
{
    this->_actList.clear();
    this->_matrix.reset();
    this->_invMatrix.reset();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphTransAffine::calcInvertedMatrix()
{
    if ( !this->_matrix.isInvertible() ) {
        this->_invMatrix.reset();
        return finErrorCodeKits::FIN_EC_PRECISE_LOSS;
    }

    this->_invMatrix = this->_matrix.inverted();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphTransAffine::appendRotate(double rad)
{
    QTransform subtrans;
    subtrans.rotate(rad);
    this->_matrix *= subtrans;
    this->calcInvertedMatrix();

    finGraphTransAffine::Action act;
    act._type = finGraphTransAffine::FIN_GTA_TYPE_ROTATE;
    act._arg1 = act._arg2 = rad;
    this->_actList.append(act);

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphTransAffine::appendScale(double sx, double sy)
{
    QTransform subtrans;
    subtrans.scale(sx, sy);
    this->_matrix *= subtrans;
    this->calcInvertedMatrix();

    finGraphTransAffine::Action act;
    act._type = finGraphTransAffine::FIN_GTA_TYPE_SCALE;
    act._arg1 = sx;
    act._arg2 = sy;
    this->_actList.append(act);

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphTransAffine::appendTranslate(double tx, double ty)
{
    QTransform subtrans;
    subtrans.translate(tx, ty);
    this->_matrix *= subtrans;
    this->calcInvertedMatrix();

    finGraphTransAffine::Action act;
    act._type = finGraphTransAffine::FIN_GTA_TYPE_TRANSLATE;
    act._arg1 = tx;
    act._arg2 = ty;
    this->_actList.append(act);

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

QPointF finGraphTransAffine::transPoint(const QPointF &ptr)
{
    return this->_matrix.map(ptr);
}

QPointF finGraphTransAffine::arcTransPoint(const QPointF &ptr)
{
    return this->_invMatrix.map(ptr);
}
