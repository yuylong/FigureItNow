/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
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
    static void fillTypesInComboBox(QComboBox *cmbox);
    static int getComboBoxIndexForType(QComboBox *cmbox, Type type);
    static void setComboBoxCurrentItemToType(QComboBox *cmbox, Type type);

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

    void setAxisZoomX(double zoomx);
    void setAxisZoomY(double zoomy);

    virtual QPointF transPoint(const QPointF &ptr);
    virtual QPointF arcTransPoint(const QPointF &ptr);
};

class finGraphTransAffine : public finGraphTrans
{
public:
    enum ActionType {
        AT_DUMMY,
        AT_ROTATE,
        AT_SCALE,
        AT_TRANSLATE
    };

    enum ActArgType {
        AAT_NONE,
        AAT_RADIAN,
        AAT_XY,
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

    void reset();
    void appendRotate(double rad);
    void appendScale(double sx, double sy);
    void appendTranslate(double tx, double ty);

    virtual QPointF transPoint(const QPointF &ptr);
    virtual QPointF arcTransPoint(const QPointF &ptr);

 private:
    void calcInvertedMatrix();
};

#endif // FINGRAPHTRANS_H
