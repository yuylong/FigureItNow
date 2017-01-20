#ifndef FINGRAPHTRANS_H
#define FINGRAPHTRANS_H

#include <QPointF>
#include <QTransform>

#include "finErrorCode.h"


class finGraphTrans
{
public:
    enum finGraphTransType {
        FIN_GT_TYPE_NONE,
        FIN_GT_TYPE_RECT,
        FIN_GT_TYPE_AFFINE,
    };

protected:
    finGraphTransType _type;

public:
    finGraphTrans();

    virtual finErrorCode cloneTransform(const finGraphTrans *trans);
    finGraphTransType getTransformType() const;
    virtual bool isLinear() const;

    virtual QPointF transPoint(const QPointF &ptr);
    virtual QPointF arcTransPoint(const QPointF &ptr);
};

typedef finGraphTrans::finGraphTransType finGraphTransType;

class finGraphTransRect : public finGraphTrans
{
protected:
    double _axisZoomX, _axisZoomY;

public:
    finGraphTransRect();

    virtual finErrorCode cloneTransform(const finGraphTrans *trans);
    virtual bool isLinear() const;

    double getAxisZoomX() const;
    double getAxisZoomY() const;

    finErrorCode setAxisZoomX(double zoomx);
    finErrorCode setAxisZoomY(double zoomy);

    virtual QPointF transPoint(const QPointF &ptr);
    virtual QPointF arcTransPoint(const QPointF &ptr);
};

class finGraphTransAffine : public finGraphTrans
{
protected:
    QTransform _matrix;
    QTransform _invMatrix;

public:
    finGraphTransAffine();

    QTransform getTransformMatrix() const;
    QTransform getInvatedTransformMatrix() const;

    virtual QPointF transPoint(const QPointF &ptr);
    virtual QPointF arcTransPoint(const QPointF &ptr);
};

#endif // FINGRAPHTRANS_H
