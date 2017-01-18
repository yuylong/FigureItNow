#ifndef FINGRAPHTRANS_H
#define FINGRAPHTRANS_H

#include <QPointF>

#include "finErrorCode.h"


class finGraphTrans
{
public:
    enum finGraphTransType {
        FIN_GT_TYPE_NONE,
        FIN_GT_TYPE_RECT,
    };

protected:
    finGraphTransType _type;

public:
    finGraphTrans();

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

    virtual bool isLinear() const;

    double getAxisZoomX() const;
    double getAxisZoomY() const;

    finErrorCode setAxisZoomX(double zoomx);
    finErrorCode setAxisZoomY(double zoomy);

    virtual QPointF transPoint(const QPointF &ptr);
    virtual QPointF arcTransPoint(const QPointF &ptr);
};

#endif // FINGRAPHTRANS_H
