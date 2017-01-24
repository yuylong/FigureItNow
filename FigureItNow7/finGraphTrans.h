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

    static QString getTransformTypeName(finGraphTransType type);
    static finGraphTransType parseTransformType(const QString &name);

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
public:
    enum ActionType {
        FIN_GTA_TYPE_ROTATE,
        FIN_GTA_TYPE_SCALE,
        FIN_GTA_TYPE_TRANSLATE
    };
    struct Action {
        ActionType _type;
        double _arg1, _arg2;
    };

protected:
    QTransform _matrix;
    QTransform _invMatrix;
    QList<Action> _actList;

public:
    finGraphTransAffine();

    virtual finErrorCode cloneTransform(const finGraphTrans *trans);
    virtual bool isLinear() const;

    QTransform getTransformMatrix() const;
    QTransform getInvertedTransformMatrix() const;

    int getActionCount() const;
    Action getActionAt(int idx) const;

    finErrorCode reset();
    finErrorCode appendRotate(double rad);
    finErrorCode appendScale(double sx, double sy);
    finErrorCode appendTranslate(double tx, double ty);

    virtual QPointF transPoint(const QPointF &ptr);
    virtual QPointF arcTransPoint(const QPointF &ptr);

 private:
    finErrorCode calcInvertedMatrix();
};

#endif // FINGRAPHTRANS_H
