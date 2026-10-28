/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 2nd, 2017
 * Copyright(c) 2017-2026 Yulong Yu. All rights reserved.
 */

#ifndef FINGRAPHCONFIG_H
#define FINGRAPHCONFIG_H

#include <QSizeF>
#include <QPointF>
#include <QColor>
#include <QRect>
#include <QBrush>
#include <QTransform>
#include <QPainter>

#include "finErrorCode.h"
#include "finFigureConfig.h"
#include "finFigurePoint3D.h"
#include "finGraphTrans.h"


/*! \class finGraphConfig
 *  \brief Describes the drawing panel and the script-to-pixel coordinate mapping.
 *
 *  The configuration owns the panel size in pixels, the background color, the pixel position of the
 *  script coordinate origin, the axis unit size, the appearance of the 3D z axis, an optional
 *  nonlinear coordinate transform, and the render hints used by the painter. Plot routines and graph
 *  panels use it to convert between script coordinates and pixel coordinates.
 */
class finGraphConfig
{
protected:
    QSizeF _panelSize;                   //!< Drawing panel size, in pixels.
    QColor _bgColor;                     //!< Panel background color.

    QPointF _originPoint;                //!< Pixel position of the script coordinate origin.
    double _axisUnitSize;                //!< Pixel length of one unit on the x/y axes.
    double _axisRadZ;                    //!< Radian direction angle of the z axis in the pixel plane.
    double _axisScaleZ;                  //!< Ratio of the z-axis unit length to the x/y unit length.
    finGraphTrans *_transform;           //!< Optional nonlinear transform; nullptr means identity.

    QPainter::RenderHints _renderHints;  //!< Render hints applied when the panel is painted.

public:
    /*! \brief Constructs a 640x480 white panel with the origin at (320, 240) and default axis
     *         settings.
     */
    finGraphConfig();
    /*! \brief Deletes the owned coordinate transform, if any. */
    ~finGraphConfig();

    /*! \brief Copies all settings from \a srccfg, including a clone of its coordinate transform. */
    void copyGraphConfig(const finGraphConfig *srccfg);

    /*! \name Panel Accessors
     *  \brief Read the panel size, background, and the rectangle covering the whole panel.
     */
    ///@{
    /*! \brief Returns the panel size in pixels. */
    const QSizeF &getPanelPixelSize() const;
    /*! \brief Returns the panel width in pixels. */
    double getPanelPixelWidth() const;
    /*! \brief Returns the panel height in pixels. */
    double getPanelPixelHeight() const;
    /*! \brief Returns the pixel rectangle covering the whole panel, starting at (0, 0). */
    QRectF getWholePanelPixelRect() const;
    /*! \brief Returns the four panel corners mapped back into script coordinates. */
    QList<QPointF> getCornerAxisPoints() const;
    /*! \brief Returns the panel background color. */
    const QColor &getBackgroundColor() const;
    /*! \brief Returns a solid brush filled with the background color. */
    QBrush getBackgroundBrush() const;
    ///@}

    /*! \name Axis and Origin Accessors
     *  \brief Read the pixel origin and the axis appearance parameters.
     */
    ///@{
    /*! \brief Returns the pixel position of the script coordinate origin. */
    const QPointF &getOriginPixelPoint() const;
    /*! \brief Returns the x coordinate of the origin in pixels. */
    double getOriginPixelPointX() const;
    /*! \brief Returns the y coordinate of the origin in pixels. */
    double getOriginPixelPointY() const;
    /*! \brief Returns the pixel length of one unit on the x/y axes. */
    double getAxisUnitPixelSize() const;
    /*! \brief Returns the radian direction angle of the z axis in the pixel plane. */
    double getAxisRadZ() const;
    /*! \brief Returns the ratio of the z-axis unit length to the x/y unit length. */
    double getAxisScaleZ() const;

    /*! \brief Returns whether the origin pixel point lies inside the whole panel rectangle. */
    bool isOriginPointOnPanel() const;
    ///@}

    /*! \name Transform Accessors
     *  \brief Read the optional nonlinear coordinate transform.
     */
    ///@{
    /*! \brief Returns the transform type; TP_NONE when no transform is installed. */
    finGraphTransType getTransformType() const;
    /*! \brief Returns the transform object, or nullptr when no transform is installed. */
    finGraphTrans *getTransform() const;
    /*! \brief Returns true when no transform is installed or the installed one is linear. */
    bool isLinearTransform() const;
    ///@}

    /*! \brief Returns the render hints applied when the panel is painted. */
    QPainter::RenderHints getRenderHints() const;

    /*! \name Panel Mutators
     *  \brief Set the panel size and background color.
     */
    ///@{
    /*! \brief Sets the panel size in pixels. */
    void setPanelPixelSize(const QSizeF &size);
    /*! \brief Sets the panel width in pixels. */
    void setPanelPixelWidth(double width);
    /*! \brief Sets the panel height in pixels. */
    void setPanelPixelHeight(double height);
    /*! \brief Sets the panel size in pixels. */
    void setPanelPixelSize(double sizex, double sizey);
    /*! \brief Sets the panel background color. */
    void setBackgroundColor(const QColor &color);
    ///@}

    /*! \name Axis and Origin Mutators
     *  \brief Set the pixel origin and the axis appearance parameters.
     */
    ///@{
    /*! \brief Sets the pixel position of the script coordinate origin. */
    void setOriginPixelPoint(const QPointF &pt);
    /*! \brief Sets the x coordinate of the origin in pixels. */
    void setOriginPixelPointX(double ptx);
    /*! \brief Sets the y coordinate of the origin in pixels. */
    void setOriginPixelPointY(double pty);
    /*! \brief Sets the pixel position of the script coordinate origin. */
    void setOriginPixelPoint(double ptx, double pty);
    /*! \brief Sets the pixel length of one unit on the x/y axes. */
    void setAxisUnitPixelSize(double size);
    /*! \brief Sets the radian direction angle of the z axis in the pixel plane. */
    void setAxisRadZ(double rad);
    /*! \brief Sets the ratio of the z-axis unit length to the x/y unit length. */
    void setAxisScaleZ(double scale);
    ///@}

    /*! \name Transform Mutators
     *  \brief Install, replace, or copy the nonlinear coordinate transform.
     */
    ///@{
    /*! \brief Replaces the transform with a new object of the given type, throwing on allocation
     *         failure.
     */
    void setTransformType(finGraphTransType type);
    /*! \brief Clones the source transform, removing the installed one when \a srctrans is null or
     *         TP_NONE.
     */
    void cloneTransform(const finGraphTrans *srctrans);
    /*! \brief Clones the coordinate transform held by the source configuration. */
    void cloneTransform(const finGraphConfig *srccfg);
    /*! \brief Returns the matrix mapping script coordinates to pixels using only the origin and the
     *         axis unit, ignoring any nonlinear transform.
     */
    QTransform getNakePixelTransformMatrix() const;
    ///@}

    /*! \brief Sets the render hints applied when the panel is painted. */
    void setRenderHints(QPainter::RenderHints hints);

    /*! \name Coordinate Conversion
     *  \brief Convert points, paths, and polygons between script and pixel coordinates.
     */
    ///@{
    /*! \brief Maps a 3D script point to pixel coordinates by shifting its (x, y) projection along
     *         the z-axis direction.
     */
    QPointF transformPixelPoint3D(double x, double y, double z) const;
    /*! \brief Maps a 3D script point to pixel coordinates. */
    QPointF transformPixelPoint3D(const finFigurePoint3D &pt) const;

    /*! \brief Maps a point in script coordinates to pixel coordinates. */
    QPointF transformPixelPoint(const QPointF &srcpt) const;
    /*! \brief Maps a point in pixel coordinates back to script coordinates. */
    QPointF arcTransformPixelPoint(const QPointF &srcpt) const;
    /*! \brief Maps every element of \a path from script coordinates to pixel coordinates. */
    QPainterPath transformPixelPath(const QPainterPath &path) const;
    /*! \brief Maps every element of \a path from pixel coordinates back to script coordinates. */
    QPainterPath arcTransformPixelPath(const QPainterPath &path) const;
    /*! \brief Maps every point of \a polygon from script coordinates to pixel coordinates. */
    QPolygonF transformPixelPolygon(const QPolygonF &polygon) const;
    /*! \brief Maps every point of \a polygon from pixel coordinates back to script coordinates. */
    QPolygonF arcTransformPixelPolygon(const QPolygonF &polygon) const;
    ///@}

    /*! \name Render Hint Names
     *  \brief Convert between render hint flags and their configuration string names.
     */
    ///@{
    /*! \brief Returns the configuration name for the given hints (e.g. "high_antialiasing"). */
    static QString getRenderHintsName(const QPainter::RenderHints &hints);
    /*! \brief Returns the render hints for the given configuration name, defaulting to
     *         QPainter::Antialiasing for unknown names.
     */
    static QPainter::RenderHints parseRenderHints(const QString &name);
    ///@}
};

#endif // FINGRAPHCONFIG_H
