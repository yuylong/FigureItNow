#include "finGraphTrans.h"

finGraphTrans::finGraphTrans()
{
    this->_type = finGraphTrans::FIN_GT_TYPE_NONE;
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
