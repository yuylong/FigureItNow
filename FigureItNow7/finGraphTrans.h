/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#ifndef FINGRAPHTRANS_H
#define FINGRAPHTRANS_H

#include <QPointF>
#include <QTransform>
#include <QComboBox>

#include "finErrorCode.h"


class finGraphTrans
{
public:
    enum Type {
        TP_NONE,
        TP_RECT,
        TP_AFFINE,
    };

    static QString getTransformTypeName(Type type);
    static Type parseTransformType(const QString &name);
    static finErrorCode fillTypesInComboBox(QComboBox *cmbox);
    static int getComboBoxIndexForType(QComboBox *cmbox, Type type);
    static finErrorCode setComboBoxCurrentItemToType(QComboBox *cmbox, Type type);

protected:
    Type _type;

public:
    finGraphTrans();
    virtual ~finGraphTrans();

    virtual finErrorCode cloneTransform(const finGraphTrans *trans);
    Type getTransformType() const;
    virtual bool isLinear() const;

    virtual QPointF transPoint(const QPointF &ptr);
    virtual QPointF arcTransPoint(const QPointF &ptr);
};

typedef finGraphTrans::Type finGraphTransType;

class finGraphTransRect : public finGraphTrans
{
protected:
    double _axisZoomX, _axisZoomY;

public:
    finGraphTransRect();
    virtual ~finGraphTransRect();

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
        FIN_GTA_TYPE_DUMMY,
        FIN_GTA_TYPE_ROTATE,
        FIN_GTA_TYPE_SCALE,
        FIN_GTA_TYPE_TRANSLATE
    };

    enum ActArgType {
        FIN_GTA_AAT_NONE,
        FIN_GTA_AAT_RADIAN,
        FIN_GTA_AAT_XY,
    };

    static QString getAffineTransActionName(ActionType type);
    static ActionType parseAffineTransAction(const QString &name);
    static finErrorCode fillAffineTransActionsInComboBox(QComboBox *cmbox);

    static int getAffineTransActionArgCnt(ActionType type);
    static ActArgType getAffineTransActionArgType(ActionType type);

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
    virtual ~finGraphTransAffine();

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
