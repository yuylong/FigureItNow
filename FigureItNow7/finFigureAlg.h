/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */
/*! \file finFigureAlg.h
 *  \brief The figuring algorithm library.
 *
 * This file declares a static class that contains all the figuring algorithms used in FIN-7 system.
 */

#ifndef FINFIGUREALG_H
#define FINFIGUREALG_H

#include <QPointF>
#include <QRectF>
#include <QList>
#include <QTransform>


/*! \struct finFigAlgLine2D
 *  \brief Coefficients of a two-dimensional implicit line.
 *
 *  The represented line satisfies \f$ax + by + c = 0\f$.
 */
struct finFigAlgLine2D {
    double a;  //!< Coefficient of the x coordinate.
    double b;  //!< Coefficient of the y coordinate.
    double c;  //!< Constant term.
};

/*! \class finFigureAlg
 *  \brief Static geometry and transformation helpers for figure construction.
 *
 *  Provides two-dimensional vector, line, polygon, rectangle, and QTransform operations used by
 *  figure objects and graph rendering code.
 */
class finFigureAlg
{
public:
    /*! \brief Constructs the utility object. */
    finFigureAlg();

    /*! \name Vector And Angle Operations
     *  \brief Calculate lengths, distances, directions, and perpendicular vectors.
     */
    ///@{
    /*! \brief Returns the Euclidean length of \a vec. */
    static double vectorLength(const QPointF &vec);
    /*! \brief Returns the Euclidean distance between two points. */
    static double pointsDistance(const QPointF &pt1, const QPointF &pt2);
    /*! \brief Moves from \a basept towards \a dirpt by \a len. */
    static QPointF movePointInside(const QPointF &basept, const QPointF &dirpt, double len);
    /*! \brief Moves from \a basept away from \a dirpt by \a len. */
    static QPointF movePointOutside(const QPointF &basept, const QPointF &dirpt, double len);

    /*! \brief Returns the signed radian angle of \a vec from the positive x-axis. */
    static double getVectorRadian(const QPointF &vec);
    /*! \brief Returns the normalized angular difference between \a rad1 and \a rad2. */
    static double radianDifference(double rad1, double rad2);
    /*! \brief Returns a clockwise perpendicular vector to \a vec with length \a len. */
    static QPointF getVerticalVector(const QPointF &vec, double len);
    ///@}

    /*! \name Line Operations
     *  \brief Construct and query lines represented by finFigAlgLine2D coefficients.
     */
    ///@{
    /*! \brief Constructs the vertical line at x = \a xval. */
    static finFigAlgLine2D vecLineFromXVal(double xval);
    /*! \brief Constructs the horizontal line at y = \a yval. */
    static finFigAlgLine2D horLineFromYVal(double yval);
    /*! \brief Constructs the line that passes through \a pt1 and \a pt2. */
    static finFigAlgLine2D line2DFromPoints(const QPointF &pt1, const QPointF &pt2);

    /*! \brief Returns whether two lines are parallel within the implementation tolerance. */
    static bool isParallelLines(const finFigAlgLine2D &line1, const finFigAlgLine2D &line2);
    /*! \brief Returns the intersection point of two non-parallel lines. */
    static QPointF lineCrossPoint(const finFigAlgLine2D &line1, const finFigAlgLine2D &line2);
    /*! \brief Evaluates the implicit equation of \a line at \a pt. */
    static double linePointValue(const finFigAlgLine2D &line, const QPointF &pt);
    /*! \brief Returns whether two points lie on the same side of \a line. */
    static bool arePointsSameSide(const finFigAlgLine2D &line, const QPointF &pt1, const QPointF &pt2);
    ///@}

    /*! \name Polygon Operations
     *  \brief Test points and clip polygons against lines or axis-aligned bounds.
     */
    ///@{
    /*! \brief Returns whether \a chkpt lies between the two endpoint projections. */
    static bool isPointBetween(const QPointF &chkpt, const QPointF &pvpt1, const QPointF &pvpt2);
    /*! \brief Returns whether \a chkpt is inside the convex \a polygon. */
    static bool isPointInsidePolygon(const QPointF &chkpt, const QList<QPointF> &polygon);
    /*! \brief Returns distinct intersections between \a line and polygon edges. */
    static QList<QPointF> polygonCrossPoint(const finFigAlgLine2D &line, const QList<QPointF> &polygon);
    /*! \brief Clips \a polygon to the half-plane x >= \a xmin. */
    static QList<QPointF> xMinCutPolygon(const QList<QPointF> &polygon, double xmin);
    /*! \brief Clips \a polygon to the half-plane x <= \a xmax. */
    static QList<QPointF> xMaxCutPolygon(const QList<QPointF> &polygon, double xmax);
    /*! \brief Clips \a polygon to the half-plane y >= \a ymin. */
    static QList<QPointF> yMinCutPolygon(const QList<QPointF> &polygon, double ymin);
    /*! \brief Clips \a polygon to the half-plane y <= \a ymax. */
    static QList<QPointF> yMaxCutPolygon(const QList<QPointF> &polygon, double ymax);
    ///@}

    /*! \name Rectangle Containment
     *  \brief Test point, rectangle, and polygon containment in an axis-aligned rectangle.
     */
    ///@{
    /*! \brief Returns whether \a pt is inside \a baserect. */
    static bool isPointInsideRect(const QPointF &pt, const QRectF &baserect);
    /*! \brief Returns whether \a rect is completely contained in \a baserect. */
    static bool isRectInsideRect(const QRectF &rect, const QRectF &baserect);
    /*! \brief Returns whether every polygon vertex is inside \a baserect. */
    static bool isPolygonInsideRect(const QList<QPointF> &polygon, const QRectF &baserect);
    ///@}

    /*! \name Transform Construction
     *  \brief Inspect matrices and create affine or projective transforms from point correspondences.
     */
    ///@{
    /*! \brief Writes the elements of \a matrix to the standard output. */
    static void dumpMatrix(const QTransform &matrix);
    /*! \brief Builds an affine transform basis from three destination points. */
    static QTransform threePointMatrix(const QPointF &pt00, const QPointF &pt10, const QPointF &pt01);
    /*! \brief Builds a transform mapping the first three points in \a fromlist to \a tolist. */
    static QTransform threePointMatrix(const QList<QPointF> &fromlist, const QList<QPointF> &tolist);
    /*! \brief Builds a projective transform basis from four destination points. */
    static QTransform fourPointMatrix(const QPointF &pt00, const QPointF &pt10, const QPointF &pt11,
                                      const QPointF &pt01);
    /*! \brief Builds a transform mapping the first four points in \a fromlist to \a tolist. */
    static QTransform fourPointMatrix(const QList<QPointF> &fromlist, const QList<QPointF> &tolist);
    ///@}
};

#endif // FINFIGUREALG_H
