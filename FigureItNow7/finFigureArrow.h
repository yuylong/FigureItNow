/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */
/*! \file finFigureArrow.h
 *  \brief The arrow object installed on figuring objects.
 *
 * This file declares a class referred as arrow to describe the arrow at the line end of the figuring object.
 */

#ifndef FINFIGUREARROW_H
#define FINFIGUREARROW_H

#include <QString>
#include <QList>
#include <QPointF>
#include <QPen>
#include <QBrush>
#include <QPainterPath>
#include <QComboBox>

#include "finErrorCode.h"
#include "finFigurePath.h"


class finFigureConfig;
class finGraphConfig;

/*! \class finFigureArrow
 *  \brief Describes an arrowhead drawn at the end of a figure line.
 *
 *  Stores the arrowhead type, opening angle, and length. It also builds the pixel-space path and
 *  calculates the shortened line endpoint needed to place an arrowhead without overlap.
 */
class finFigureArrow
{
public:
    /*! \enum finFigureArrow::Type
     *  \brief Supported arrowhead shapes.
     */
    enum Type {
        TP_NONE,      //!< No arrowhead is drawn.
        TP_TRIANGLE,  //!< A closed triangular arrowhead is drawn.
    };

    /*! \name Type Helpers
     *  \brief Convert arrow types for configuration and populate UI selectors.
     */
    ///@{
    /*! \brief Returns the configuration name of \a type. */
    static QString getTypeName(finFigureArrow::Type type);
    /*! \brief Parses a configuration string as an arrow type. */
    static finFigureArrow::Type parseTypeString(const QString &str);
    /*! \brief Adds all supported arrow types to \a cmbox. */
    static finErrorCode fillTypesInComboBox(QComboBox *cmbox);
    ///@}

protected:
    finFigureArrow::Type _type;  //!< Arrowhead shape.
    double _rad;                  //!< Triangle opening angle in radians.
    double _length;               //!< Arrowhead length in pixels.

public:
    /*! \brief Constructs a disabled arrow with default triangle dimensions. */
    finFigureArrow();
    /*! \brief Constructs an arrow by copying \a arrow. */
    finFigureArrow(const finFigureArrow &arrow);

    /*! \name Arrow Properties
     *  \brief Read and configure the selected arrowhead type and dimensions.
     */
    ///@{
    /*! \brief Returns the selected arrowhead type. */
    finFigureArrow::Type getType() const;
    /*! \brief Returns the opening angle, or zero when no arrowhead is enabled. */
    double getRadian() const;
    /*! \brief Returns the arrowhead length, or zero when no arrowhead is enabled. */
    double getLength() const;

    /*! \brief Sets the arrowhead type. */
    void setType(finFigureArrow::Type type);
    /*! \brief Sets and normalizes the triangle opening angle in radians. */
    void setRadian(double rad);
    /*! \brief Sets the non-negative arrowhead length in pixels. */
    void setLength(double length);

    /*! \brief Replaces this arrow's properties with those of \a arrow. */
    finFigureArrow &operator = (const finFigureArrow &arrow);
    ///@}

    /*! \name Rendering Helpers
     *  \brief Derive drawing styles and geometry from the figure configuration.
     */
    ///@{
    /*! \brief Returns the arrow outline pen adapted from \a cfg's border pen. */
    QPen getArrowPen(const finFigureConfig *cfg) const;
    /*! \brief Returns the arrow fill brush derived from \a cfg's border pen. */
    QBrush getArrowBrush(const finFigureConfig *cfg) const;

    /*! \brief Returns the line endpoint after reserving space for the arrowhead. */
    QPointF lineShrinkPoint(const QPointF &arwpt, const QPointF &prevpt, const finFigureConfig *cfg) const;
    /*! \brief Appends the arrowhead's pixel-space paths to \a pathlist. */
    void getPixelPath(QList<finFigurePath> *pathlist, const QPointF &arwpt, const QPointF &prevpt,
                      const finFigureConfig *cfg) const;
    ///@}

private:
    /*! \brief Calculates the triangular arrowhead's line-shrink distance. */
    double lineShrinkLenTriangle(const finFigureConfig *cfg) const;
    /*! \brief Calculates the shortened endpoint for a triangular arrowhead. */
    QPointF lineShrinkPtTriangle(const QPointF &prevpt, const QPointF &arwpt, const finFigureConfig *cfg) const;
    /*! \brief Appends a closed triangular arrowhead path to \a pathlist. */
    void getPixelPathTriangle(QList<finFigurePath> *pathlist, const QPointF &arwpt, const QPointF &prevpt,
                              const finFigureConfig *cfg) const;
};

typedef finFigureArrow::Type finFigureArrowType;

#endif // FINFIGUREARROW_H
