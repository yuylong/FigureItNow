/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 31st, 2016
 * Copyright(c) 2015-2025 Yulong Yu. All rights reserved.
 */

#ifndef FINFIGUREOBJECT_H
#define FINFIGUREOBJECT_H

#include <QString>
#include <QList>
#include <QPointF>
#include <QSizeF>
#include <QPainterPath>

#include "finErrorCode.h"
#include "finFigureConfig.h"
#include "finFigurePoint3D.h"
#include "finFigurePath.h"
#include "finGraphConfig.h"


class finFigureObject
{
public:
    enum Type {
        TP_DUMMY,
        TP_DOT,
        TP_LINE,
        TP_POLYLINE,
        TP_RECT,
        TP_POLYGON,
        TP_ELLIPSE,
        TP_TEXT,
        TP_IMAGE,
        TP_AXIS,
        TP_LINE3D,
        TP_MAX
    };

protected:
    Type _type;
    finFigureConfig _figCfg;

public:
    finFigureObject();
    virtual ~finFigureObject();

    virtual bool is3DFigure() const;

    Type getFigureType() const;
    const finFigureConfig *getFigureConfig() const;
    finFigureConfig *getFigureConfig();

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;
};

typedef finFigureObject::Type finFigureObjectType;

class finFigureObjectDot : public finFigureObject
{
protected:
    QPointF _point;

public:
    finFigureObjectDot();
    virtual ~finFigureObjectDot();

    virtual bool is3DFigure() const;

    QPointF getPoint() const;

    void setPoint(const QPointF &qpt);
    void setPoint(double ptx, double pty);

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;
};

class finFigureObjectLine : public finFigureObject
{
protected:
    QPointF _pt1, _pt2;
    bool _ignoreArrow;

public:
    finFigureObjectLine();
    virtual ~finFigureObjectLine();

    virtual bool is3DFigure() const;

    QPointF getPoint1() const;
    QPointF getPoint2() const;

    void setPoint1(const QPointF &qpt);
    void setPoint1(double ptx, double pty);
    void setPoint2(const QPointF &qpt);
    void setPoint2(double ptx, double pty);

    bool isArrowIgnored() const;
    void setIgnoreArrow(bool blval);

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;
};

class finFigureObjectPolyline : public finFigureObject
{
protected:
    QList<QPointF> _ptList;
    bool _ignoreArrow;

public:
    finFigureObjectPolyline();
    virtual ~finFigureObjectPolyline();

    virtual bool is3DFigure() const;

    int getPointCount() const;
    QPointF getPointAt(int idx) const;

    void appendPoint(const QPointF &pt);
    void appendPoint(double ptx, double pty);
    void appendPoints(const QList<QPointF> &ptlist);
    void removePointAt(int idx);

    bool isArrowIgnored() const;
    void setIgnoreArrow(bool blval);

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;

private:
    QList<QPointF> getTransformedPointList(finGraphConfig *cfg) const;
};

class finFigureObjectRect : public finFigureObject
{
protected:
    QPointF _center;
    QSizeF _size;
    double _radian;
    double _sinrad, _cosrad;

public:
    finFigureObjectRect();
    virtual ~finFigureObjectRect();

    virtual bool is3DFigure() const;

    QPointF getCenterPoint() const;
    QSizeF getSize() const;
    double getRadian() const;
    QPointF getUpperLeftPoint() const;
    QPointF getUpperRightPoint() const;
    QPointF getLowerLeftPoint() const;
    QPointF getLowerRightPoint() const;

    void setCenterPoint(const QPointF &ctrpt);
    void setCenterPoint(double cx, double cy);
    void setSize(const QSizeF &size);
    void setSize(double width, double height);
    void setRadian(double rad);

    QPolygonF getPolygonInstance() const;

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;
};

class finFigureObjectPolygon : public finFigureObject
{
protected:
    QList<QPointF> _ptList;

public:
    finFigureObjectPolygon();
    virtual ~finFigureObjectPolygon();

    virtual bool is3DFigure() const;

    int getPointCount() const;
    QPointF getPointAt(int idx) const;

    void appendPoint(const QPointF &pt);
    void appendPoint(double ptx, double pty);
    void appendPoints(const QList<QPointF> &ptlist);
    void removePointAt(int idx);

    QPolygonF getPolygonInstance() const;

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;
};

class finFigureObjectEllipse : public finFigureObject
{
protected:
    QPointF _center;
    double _longR, _shortR;
    double _radian;
    double _sinrad, _cosrad;

public:
    finFigureObjectEllipse();
    virtual ~finFigureObjectEllipse();

    virtual bool is3DFigure() const;

    QPointF getCenterPoint() const;
    double getLongRadius() const;
    double getShortRadius() const;
    double getRadian() const;

    void setCenterPoint(const QPointF &ctrpt);
    void setCenterPoint(double cx, double cy);
    void setLongRadius(double lr);
    void setShortRadius(double sr);
    void setRadian(double rad);

    QPointF getEllipsePointAtRad(double rad) const;

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;
};

class finFigureObjectText : public finFigureObject
{
protected:
    QPointF _basePtr;
    int _flag;
    double _scale;
    double _rad;
    double _sinrad, _cosrad;

    QString _text;
    bool _isPinned;

public:
    finFigureObjectText();
    virtual ~finFigureObjectText();

    virtual bool is3DFigure() const;

    QPointF getBasePoint() const;
    int getFontMetricFlags() const;
    double getScale() const;
    double getRadian() const;
    QString getText() const;
    bool isPinned() const;

    finErrorCode setBasePoint(const QPointF &pt);
    finErrorCode setBasePoint(double ptx, double pty);
    finErrorCode setFontMetricFlags(int flag);
    finErrorCode setScale(double scale);
    finErrorCode setRadian(double rad);
    finErrorCode setText(const QString &text);
    finErrorCode setIsPinned(bool pinned);

    QRectF getBoundingRect() const;
    QPainterPath getPixelTextPath(finGraphConfig *cfg) const;

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;

private:
    QPointF getBasePointOffset(const QRectF &boundrect) const;
    QPainterPath getPinnedPixelTextPath(finGraphConfig *cfg) const;
    QPainterPath getUnpinnedPixelTextPath(finGraphConfig *cfg) const;
};

class finFigureObjectImage : public finFigureObject
{
protected:
    QPointF _basePtr;
    int _flag;
    double _scaleX, _scaleY;
    double _rad;
    double _sinrad, _cosrad;

    QImage _img;
    bool _isPinned;

public:
    finFigureObjectImage();
    virtual ~finFigureObjectImage();

    virtual bool is3DFigure() const;

    QPointF getBasePoint() const;
    int getAlignFlags() const;
    double getScaleX() const;
    double getScaleY() const;
    double getRadian() const;
    QImage getImage() const;
    bool isPinned() const;

    finErrorCode setBasePoint(const QPointF &pt);
    finErrorCode setBasePoint(double ptx, double pty);
    finErrorCode setAlignFlags(int flag);
    finErrorCode setScaleX(double scale);
    finErrorCode setScaleY(double scale);
    finErrorCode setRadian(double rad);
    finErrorCode setImage(const QImage &image);
    finErrorCode setIsPinned(bool pinned);

    QRectF getBoundingRect() const;

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;

private:
    QTransform getParameterTransformMatrix() const;
    finErrorCode getUnpinnedPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    finErrorCode getPinnedPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
};

class finFigureObjectAxis : public finFigureObject
{
protected:
    double _minX, _maxX, _minY, _maxY;
    double _stepX, _stepY;
    QString _titleX, _titleY;

    static double _defMinX, _defMaxX, _defMinY, _defMaxY;

public:
    finFigureObjectAxis();
    virtual ~finFigureObjectAxis();

    virtual bool is3DFigure() const;

    double getRangeMinX() const;
    double getRangeMaxX() const;
    double getRangeMinY() const;
    double getRangeMaxY() const;
    bool isAutoRangeX() const;
    bool isAutoRangeY() const;

    double getStepX() const;
    double getStepY() const;
    bool isAutoStepX() const;
    bool isAutoStepY() const;

    QString getTitleX() const;
    QString getTitleY() const;

    finErrorCode setAutoRangeX();
    finErrorCode setRangeX(double minx, double maxx);
    finErrorCode setAutoRangeY();
    finErrorCode setRangeY(double miny, double maxy);

    finErrorCode setAutoStepX();
    finErrorCode setStepX(double step);
    finErrorCode setAutoStepY();
    finErrorCode setStepY(double step);

    finErrorCode setTitleX(const QString &title);
    finErrorCode setTitleY(const QString &title);

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;

private:
    static QRectF getDefaultRangeRect();

    double getSecondMinNum(double *ary, int cnt) const;
    double getSecondMaxNum(double *ary, int cnt) const;
    QRectF getCrossPointRange(const QList<QPointF> &panelrect) const;
    double getGivenAxisCrossPosition(double minnum, double maxnum) const;
    QPointF getAxisCrossPoint(const QList<QPointF> &panelrect) const;
    QRectF getAxisDrawRange(const QList<QPointF> &panelrect, const QPointF &crosspt) const;

    double getTickStep(bool isx, const QPointF &crosspt, finGraphConfig *cfg) const;
    QPointF getStepPixelVector(const QPointF &steppt, const QPointF &crosspt, finGraphConfig *cfg) const;
    QPointF getTickPixelVector(const QPointF &steppixvec, double ticksize) const;
    QPointF getTickPixelVector(const QPointF &steppixvec) const;
    double getAxisPixelRadian(const QPointF &steppixvec) const;
    finErrorCode setupTickLabel(const QPointF &steppixvec, finFigureObjectText *fotext) const;

    QPainterPath getAxisTitlePath(const QPointF &axisstartpt, const QPointF &axisendpt,
                                  const QString &title, finGraphConfig *cfg) const;

    finErrorCode getLinesPath(QList<finFigurePath> *pathlist, finGraphConfig *cfg,
                              const QPointF &crosspt, const QRectF &drawrange) const;
    finErrorCode getTickPath(QList<finFigurePath> *pathlist, finGraphConfig *cfg,
                             const QPointF &crosspt, const QRectF &drawrange) const;
    finErrorCode getTitlePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg,
                              const QPointF &crosspt, const QRectF &drawrange) const;
};

class finFigureObjectLine3D : public finFigureObject
{
protected:
    finFigurePoint3D _pt1, _pt2;

public:
    finFigureObjectLine3D();
    virtual ~finFigureObjectLine3D();

    virtual bool is3DFigure() const;

    finFigurePoint3D getPoint1() const;
    finFigurePoint3D getPoint2() const;

    finErrorCode setPoint1(const finFigurePoint3D &qpt);
    finErrorCode setPoint1(double ptx, double pty, double ptz);
    finErrorCode setPoint2(const finFigurePoint3D &qpt);
    finErrorCode setPoint2(double ptx, double pty, double ptz);

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;
};

#endif // FINFIGUREOBJECT_H
