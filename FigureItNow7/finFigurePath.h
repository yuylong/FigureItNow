/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finFigurePath.h
 *  \brief Declaration of a styled vector-path or bitmap rendering payload.
 *
 *  finFigurePath is the intermediate data object produced by figure primitives and consumed by graph
 *  panel renderers. It stores drawing styles, a QPainterPath, and an optional positioned raster image.
 */

#ifndef FINFIGUREPATH_H
#define FINFIGUREPATH_H

#include <QPen>
#include <QBrush>
#include <QPainterPath>
#include <QPointF>
#include <QPixmap>
#include <QImage>

#include "finErrorCode.h"


/*! \class finFigurePath
 *  \brief Holds the styled pixel-space output of one figure rendering operation.
 *
 *  A path instance may carry vector geometry in a QPainterPath, a raster image positioned by its
 *  top-left pixel coordinate, or both, together with the pen and brush used to render it.
 */
class finFigurePath
{
protected:
    QPen _pen;          //!< Pen used to draw the vector path.
    QBrush _brush;      //!< Brush used to fill the vector path.
    QPainterPath _path; //!< Pixel-space vector geometry.
    QPointF _imgPos;    //!< Top-left pixel position of the optional raster image.
    QPixmap _pixmap;    //!< Optional raster payload.

public:
    /*! \brief Constructs an empty payload with a black pen and transparent brush. */
    finFigurePath();
    /*! \brief Constructs a payload by copying \a figpath. */
    finFigurePath(const finFigurePath &figpath);

    /*! \name Rendering Payload Accessors
     *  \brief Read the vector drawing styles, geometry, and optional raster payload.
     */
    ///@{
    /*! \brief Returns the vector-path pen. */
    const QPen &getPen() const;
    /*! \brief Returns the vector-path fill brush. */
    const QBrush &getBrush() const;
    /*! \brief Returns the pixel-space vector path. */
    const QPainterPath &getPath() const;
    /*! \brief Returns the top-left pixel position of the raster payload. */
    const QPointF &getImagePosition() const;
    /*! \brief Returns the raster payload as a pixmap. */
    const QPixmap &getPixmap() const;
    /*! \brief Returns the raster payload converted to an image. */
    QImage getImage() const;
    ///@}

    /*! \name Rendering Payload Mutators
     *  \brief Replace the vector drawing styles, geometry, and optional raster payload.
     */
    ///@{
    /*! \brief Sets the vector-path pen. */
    void setPen(const QPen &pen);
    /*! \brief Sets the vector-path fill brush. */
    void setBrush(const QBrush &brush);
    /*! \brief Sets the pixel-space vector path. */
    void setPath(const QPainterPath &path);
    /*! \brief Sets the top-left pixel position of the raster payload. */
    void setImagePosition(const QPointF &imgpos);
    /*! \brief Sets the raster payload from a pixmap. */
    void setPixmap(const QPixmap &pixmap);
    /*! \brief Converts \a image to a pixmap and stores it as the raster payload. */
    void setImage(const QImage &image);
    ///@}

    /*! \brief Replaces this rendering payload with a copy of \a figpath. */
    finFigurePath &operator = (const finFigurePath &figpath);
};

#endif // FINFIGUREPATH_H
