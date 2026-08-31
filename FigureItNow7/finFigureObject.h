/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 31st, 2016
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finFigureObject.h
 *  \brief Declarations of drawable FIN figure objects.
 *
 *  Defines the base figure contract and two- and three-dimensional primitives that turn mathematical
 *  geometry into finFigurePath instances using a finGraphConfig coordinate transformation.
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


/*! \class finFigureObject
 *  \brief Abstract base class for a drawable FIN figure primitive.
 *
 *  Derived classes retain their geometry in mathematical coordinates and generate styled pixel-space
 *  paths for the graph panel. Every figure object owns an independent finFigureConfig.
 */
class finFigureObject
{
public:
    /*! \enum finFigureObject::Type
     *  \brief Kinds of figure primitives supported by the rendering layer.
     */
    enum Type {
        TP_DUMMY,     //!< Placeholder base-object type.
        TP_DOT,       //!< A two-dimensional point marker.
        TP_LINE,      //!< A two-dimensional line segment.
        TP_POLYLINE,  //!< An open sequence of connected line segments.
        TP_RECT,      //!< A rotated rectangle.
        TP_POLYGON,   //!< A closed polygon.
        TP_ELLIPSE,   //!< A rotated ellipse.
        TP_TEXT,      //!< A text glyph path.
        TP_IMAGE,     //!< A transformed raster image.
        TP_AXIS,      //!< A pair of coordinate axes with ticks and titles.
        TP_LINE3D,    //!< A three-dimensional line segment.
        TP_MAX        //!< Sentinel marking the end of the enum.
    };

protected:
    Type _type;               //!< Concrete figure primitive type.
    finFigureConfig _figCfg;  //!< Drawing and text styles owned by this figure.

public:
    /*! \brief Constructs a dummy base figure object. */
    finFigureObject();
    /*! \brief Destroys the figure object through its polymorphic interface. */
    virtual ~finFigureObject();

    /*! \brief Returns whether this figure uses three-dimensional coordinates. */
    virtual bool is3DFigure() const;

    /*! \brief Returns the concrete primitive type. */
    Type getFigureType() const;
    /*! \brief Returns this figure's immutable drawing configuration. */
    const finFigureConfig *getFigureConfig() const;
    /*! \brief Returns this figure's mutable drawing configuration. */
    finFigureConfig *getFigureConfig();

    /*! \brief Appends styled pixel-space paths representing this figure to \a pathlist. */
    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    /*! \brief Writes a diagnostic representation of the figure to standard output. */
    virtual void dump() const;
};

typedef finFigureObject::Type finFigureObjectType;

/*! \class finFigureObjectDot
 *  \brief A styled two-dimensional point marker.
 */
class finFigureObjectDot : public finFigureObject
{
protected:
    QPointF _point;  //!< Point location in mathematical coordinates.

public:
    finFigureObjectDot();
    virtual ~finFigureObjectDot();

    virtual bool is3DFigure() const;

    /*! \brief Returns the marker location. */
    QPointF getPoint() const;

    /*! \brief Sets the marker location. */
    void setPoint(const QPointF &qpt);
    /*! \brief Sets the marker location from coordinates. */
    void setPoint(double ptx, double pty);

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;
};

/*! \class finFigureObjectLine
 *  \brief A two-dimensional line segment with optional end arrowheads.
 */
class finFigureObjectLine : public finFigureObject
{
protected:
    QPointF _pt1, _pt2;  //!< Start and end locations in mathematical coordinates.
    bool _ignoreArrow;   //!< Whether configured endpoint arrows are suppressed.

public:
    finFigureObjectLine();
    virtual ~finFigureObjectLine();

    virtual bool is3DFigure() const;

    /*! \brief Returns the start point. */
    QPointF getPoint1() const;
    /*! \brief Returns the end point. */
    QPointF getPoint2() const;

    /*! \brief Sets the start point. */
    void setPoint1(const QPointF &qpt);
    /*! \brief Sets the start point from coordinates. */
    void setPoint1(double ptx, double pty);
    /*! \brief Sets the end point. */
    void setPoint2(const QPointF &qpt);
    /*! \brief Sets the end point from coordinates. */
    void setPoint2(double ptx, double pty);

    /*! \brief Returns whether configured endpoint arrows are suppressed. */
    bool isArrowIgnored() const;
    /*! \brief Enables or disables endpoint arrow rendering. */
    void setIgnoreArrow(bool blval);

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;
};

/*! \class finFigureObjectPolyline
 *  \brief An open sequence of two-dimensional segments with optional endpoint arrows.
 */
class finFigureObjectPolyline : public finFigureObject
{
protected:
    QList<QPointF> _ptList;  //!< Vertices in mathematical coordinate order.
    bool _ignoreArrow;       //!< Whether configured endpoint arrows are suppressed.

public:
    finFigureObjectPolyline();
    virtual ~finFigureObjectPolyline();

    virtual bool is3DFigure() const;

    /*! \brief Returns the number of vertices. */
    int getPointCount() const;
    /*! \brief Returns the vertex at \a idx. */
    QPointF getPointAt(int idx) const;

    /*! \brief Appends one vertex. */
    void appendPoint(const QPointF &pt);
    /*! \brief Appends one vertex from coordinates. */
    void appendPoint(double ptx, double pty);
    /*! \brief Appends all vertices in \a ptlist. */
    void appendPoints(const QList<QPointF> &ptlist);
    /*! \brief Removes the vertex at \a idx. */
    void removePointAt(int idx);

    bool isArrowIgnored() const;
    void setIgnoreArrow(bool blval);

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;

private:
    /*! \brief Transforms all mathematical vertices into pixel coordinates. */
    QList<QPointF> getTransformedPointList(finGraphConfig *cfg) const;
};

/*! \class finFigureObjectRect
 *  \brief A rectangle defined by its center, dimensions, and rotation.
 */
class finFigureObjectRect : public finFigureObject
{
protected:
    QPointF _center;         //!< Rectangle center in mathematical coordinates.
    QSizeF _size;            //!< Unrotated width and height.
    double _radian;          //!< Rotation angle in radians.
    double _sinrad, _cosrad; //!< Cached trigonometric values for corner calculations.

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

    /*! \brief Returns the closed, rotated rectangle polygon. */
    QPolygonF getPolygonInstance() const;

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;
};

/*! \class finFigureObjectPolygon
 *  \brief A closed two-dimensional polygon with a configurable border and fill.
 */
class finFigureObjectPolygon : public finFigureObject
{
protected:
    QList<QPointF> _ptList;  //!< Polygon vertices in mathematical coordinate order.

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

    /*! \brief Returns the vertex list as a closed QPolygonF. */
    QPolygonF getPolygonInstance() const;

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;
};

/*! \class finFigureObjectEllipse
 *  \brief An ellipse defined by its center, radii, and rotation.
 */
class finFigureObjectEllipse : public finFigureObject
{
protected:
    QPointF _center;         //!< Ellipse center in mathematical coordinates.
    double _longR, _shortR;  //!< Horizontal and vertical radii before rotation.
    double _radian;          //!< Rotation angle in radians.
    double _sinrad, _cosrad; //!< Cached trigonometric values for point calculations.

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

    /*! \brief Returns the point on the rotated ellipse at parameter angle \a rad. */
    QPointF getEllipsePointAtRad(double rad) const;

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;
};

/*! \class finFigureObjectText
 *  \brief A rotated and aligned text figure anchored at a mathematical point.
 *
 *  Pinned text scales with the mathematical coordinate system; unpinned text keeps its pixel size.
 */
class finFigureObjectText : public finFigureObject
{
protected:
    QPointF _basePtr;        //!< Alignment anchor in mathematical coordinates.
    int _flag;               //!< Qt alignment flags for the text bounding rectangle.
    double _scale;           //!< Text scale factor.
    double _rad;             //!< Text rotation angle in radians.
    double _sinrad, _cosrad; //!< Cached trigonometric rotation values.

    QString _text;           //!< Text content to render.
    bool _isPinned;          //!< Whether text scales with the mathematical coordinate system.

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

    /*! \brief Returns the styled text bounds including configured margins. */
    QRectF getBoundingRect() const;
    /*! \brief Returns the text converted to a pixel-space painter path. */
    QPainterPath getPixelTextPath(finGraphConfig *cfg) const;

    virtual finErrorCode getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    virtual void dump() const;

private:
    /*! \brief Calculates the offset that applies the selected text alignment. */
    QPointF getBasePointOffset(const QRectF &boundrect) const;
    /*! \brief Builds the path for text pinned to mathematical coordinates. */
    QPainterPath getPinnedPixelTextPath(finGraphConfig *cfg) const;
    /*! \brief Builds the path for text with a fixed pixel size. */
    QPainterPath getUnpinnedPixelTextPath(finGraphConfig *cfg) const;
};

/*! \class finFigureObjectImage
 *  \brief A raster image positioned, scaled, rotated, and aligned on the graph.
 */
class finFigureObjectImage : public finFigureObject
{
protected:
    QPointF _basePtr;          //!< Alignment anchor in mathematical coordinates.
    int _flag;                 //!< Qt alignment flags for the image.
    double _scaleX, _scaleY;   //!< Horizontal and vertical scale factors.
    double _rad;               //!< Rotation angle in radians.
    double _sinrad, _cosrad;   //!< Cached trigonometric rotation values.

    QImage _img;               //!< Source raster image.
    bool _isPinned;            //!< Whether image scales with mathematical coordinates.

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
    /*! \brief Builds the coordinate-space scale and rotation transform around the base point. */
    QTransform getParameterTransformMatrix() const;
    /*! \brief Appends a fixed-pixel-size transformed image path. */
    finErrorCode getUnpinnedPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
    /*! \brief Appends an image path transformed with the mathematical coordinate system. */
    finErrorCode getPinnedPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const;
};

/*! \class finFigureObjectAxis
 *  \brief A coordinate-axis figure with optional explicit ranges, ticks, and titles.
 */
class finFigureObjectAxis : public finFigureObject
{
protected:
    double _minX, _maxX, _minY, _maxY;  //!< Optional explicit visible ranges for both axes.
    double _stepX, _stepY;               //!< Optional tick steps; non-positive requests automatic values.
    QString _titleX, _titleY;            //!< Labels rendered near the axis endpoints.

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
    /*! \brief Returns the fallback mathematical range used when panel geometry is unavailable. */
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

/*! \class finFigureObjectLine3D
 *  \brief A line segment between two three-dimensional mathematical points.
 */
class finFigureObjectLine3D : public finFigureObject
{
protected:
    finFigurePoint3D _pt1, _pt2;  //!< Start and end points in three-dimensional coordinates.

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
