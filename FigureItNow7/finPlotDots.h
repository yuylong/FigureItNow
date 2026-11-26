/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finPlotDots.h
 *  \brief Declarations of the point-collection plotters used to build dots, lines, and scatter figures.
 *
 *  All plotters in this header share the same point buffer (finPlotDots) and differ only in how the
 *  buffered points are converted into figure objects: one dot per point, a single polyline, a stream
 *  split into several polylines, or an unordered point cloud reconnected into continuous curves.
 */

#ifndef FINPLOTDOTS_H
#define FINPLOTDOTS_H

#include <QList>
#include <QPointF>

#include "finFigureObject.h"
#include "finFigureContainer.h"


/*! \class finPlotDots
 *  \brief Base plotter that owns an ordered point buffer and emits one dot per point.
 *
 *  A plotter never owns the target finFigureContainer; the container must be supplied with
 *  setFigureContainer() before plot() is called, otherwise plot() returns
 *  finErrorKits::EC_STATE_ERROR. Derived classes reuse the same buffer and override plot() to change
 *  the geometry that is produced.
 */
class finPlotDots
{
protected:
    QList<QPointF> _ptList;            //!< Points to plot, in the order they were appended.
    finFigureContainer *_figcontainer; //!< Target figure container; borrowed, not owned.

public:
    /*! \brief Constructs an empty plotter with no target figure container. */
    finPlotDots();
    /*! \brief Destroys the plotter and clears its point buffer. */
    virtual ~finPlotDots();

    /*! \brief Returns the number of buffered points. */
    int getPointCount() const;
    /*! \brief Returns the point at \a idx; returns the origin if \a idx is out of range. */
    const QPointF &getPointAt(int idx) const;
    /*! \brief Returns the target figure container, or \c nullptr if none has been set. */
    finFigureContainer *getFigureContainer() const;

    /*! \brief Appends \a pt to the end of the point buffer. */
    void appendPoint(const QPointF &pt);
    /*! \brief Appends the point (\a ptx, \a pty) to the end of the point buffer. */
    void appendPoint(double ptx, double pty);
    /*! \brief Appends all points in \a ptlist, preserving their order. */
    void appendPoints(const QList<QPointF> &ptlist);
    /*! \brief Inserts \a pt at the beginning of the point buffer. */
    void prependPoint(const QPointF &pt);
    /*! \brief Inserts the point (\a ptx, \a pty) at the beginning of the point buffer. */
    void prependPoint(double ptx, double pty);
    /*! \brief Inserts all points in \a ptlist ahead of the existing points, preserving their order. */
    void prependPoints(const QList<QPointF> &ptlist);
    /*! \brief Removes every buffered point. */
    void clearPoints();

    /*! \brief Sets the target container that receives the generated figures. */
    void setFigureContainer(finFigureContainer *figcontainer);

    /*! \brief Emits one finFigureObjectDot per buffered point into the figure container.
     *  \return finErrorKits::EC_SUCCESS on success; finErrorKits::EC_STATE_ERROR if no figure
     *          container has been set.
     */
    virtual finErrorCode plot();
};

/*! \class finPlotDotsLine
 *  \brief Plotter that joins the whole point buffer into a single finFigureObjectPolyline.
 *
 *  An empty buffer produces nothing, a single point is emitted as one dot, and two or more points are
 *  emitted as one arrow-less polyline through the buffered points.
 */
class finPlotDotsLine : public finPlotDots
{
public:
    /*! \brief Constructs a line plotter. */
    finPlotDotsLine();
    /*! \brief Destroys the line plotter. */
    virtual ~finPlotDotsLine();

    /*! \brief Emits the buffered points as a dot (fewer than two points) or a single polyline.
     *  \return finErrorKits::EC_SUCCESS on success; finErrorKits::EC_STATE_ERROR if no figure
     *          container has been set.
     */
    virtual finErrorCode plot() override;
};

/*! \class finPlotDotsStream
 *  \brief Plotter that splits the point buffer into several polylines.
 *
 *  The buffer is walked in order and a new polyline is started when the current point is a registered
 *  break point, is not a finite number (NaN, or infinity in both coordinates), or when the travel
 *  direction reverses sharply (more than \c 0.4999*pi radians). Points containing a single infinite
 *  coordinate are clamped to \c 65535 or \c -65535 before being plotted.
 */
class finPlotDotsStream : public finPlotDots
{
protected:
    QList<QPointF> _breakPtList; //!< Points at which the stream is forced to start a new polyline.

public:
    /*! \brief Constructs a stream plotter with an empty break-point list. */
    finPlotDotsStream();
    /*! \brief Destroys the stream plotter. */
    virtual ~finPlotDotsStream();

    /*! \brief Returns the number of registered break points. */
    int getBreakPointCount() const;
    /*! \brief Returns the break point at \a idx; returns the origin if \a idx is out of range. */
    const QPointF &getBreakPointAt(int idx) const;
    /*! \brief Returns \c true if \a pt is a registered break point. */
    bool checkBreakPoint(const QPointF &pt) const;

    /*! \brief Registers \a pt as a break point that starts a new polyline. */
    void appendBreakPoint(const QPointF &pt);
    /*! \brief Registers the point (\a ptx, \a pty) as a break point that starts a new polyline. */
    void appendBreakPoint(double ptx, double pty);
    /*! \brief Removes every registered break point. */
    void clearBreakPoints();

    /*! \brief Emits the buffered points as a sequence of polylines split at break points and
     *         discontinuities.
     *  \return finErrorKits::EC_SUCCESS on success; finErrorKits::EC_STATE_ERROR if no figure
     *          container has been set.
     */
    virtual finErrorCode plot() override;

private:
    /*! \brief Searches the break-point list for \a pt starting at \a fromidx.
     *  \param pt       The point to look for.
     *  \param fromidx  Index in the break-point list at which the search starts.
     *  \param nextidx  Output that receives the index to search from on the next call, i.e. one past the
     *                  matched entry, or \a fromidx when nothing matched. May be \c nullptr.
     *  \return \c true if \a pt was found.
     */
    bool checkBreakPointFrom(const QPointF &pt, int fromidx, int *nextidx) const;
    /*! \brief Clamps infinite coordinates of \a pt in place and reports whether it was non-finite.
     *  \param pt  The point to normalize; modified when a single coordinate is infinite.
     *  \return \c true if \a pt is a NaN point or contains an infinite coordinate.
     */
    bool checkNaNOrInfPoint(QPointF *pt);
    /*! \brief Returns \c true if \a pt must not be plotted (NaN, or infinity in both coordinates). */
    bool isNanPoint(const QPointF &pt);
};

/*! \class finPlotDotsScatter
 *  \brief Plotter that reconnects an unordered point cloud into continuous curves.
 *
 *  Starting from the first remaining point, the plotter repeatedly hops to the nearest unvisited point
 *  within the distance limit, producing one continuous curve per pass. Nearby points from earlier passes
 *  are then attached to the two ends of the curve so that adjacent curves visually join up, which is what
 *  makes scattered marching-squares style output render as connected level curves.
 */
class finPlotDotsScatter : public finPlotDots
{
protected:
    double _distLimit; //!< Maximum distance, in figure coordinates, between two connected points.

public:
    /*! \brief Constructs a scatter plotter with the default distance limit of \c 0.1. */
    finPlotDotsScatter();
    /*! \brief Destroys the scatter plotter. */
    virtual ~finPlotDotsScatter();

    /*! \brief Returns the maximum distance between two points that may be connected. */
    double getDistanceLimit() const;
    /*! \brief Sets the maximum connection distance; values below \c 1.0e-8 are raised to \c 1.0e-8. */
    void setDistanceLimit(double limit);

    /*! \brief Emits the buffered points as connected curves.
     *  \return finErrorKits::EC_SUCCESS on success; finErrorKits::EC_STATE_ERROR if no figure
     *          container has been set.
     */
    virtual finErrorCode plot() override;

private:
    /*! \brief Returns \c true if \a pt is NaN or has an infinite coordinate. */
    bool isNaNOrInfPoint(const QPointF &pt) const;

    /*! \brief Finds the point of \a ptlist nearest to \a chkpt within the distance limit.
     *  \param chkpt     The reference point.
     *  \param ptlist    The candidate points.
     *  \param exceptcnt Number of trailing entries in \a ptlist that are excluded from the search.
     *  \param outpt     Output that receives the nearest point when one is found. May be \c nullptr.
     *  \return The index of the nearest point in \a ptlist, or \c -1 if none is within range.
     */
    int findNearestPoint(const QPointF &chkpt, const QList<QPointF> &ptlist,
                         int exceptcnt = 0, QPointF *outpt = nullptr);
    /*! \brief Like findNearestPoint(), but only accepts points that continue the travel direction.
     *
     *  A candidate is skipped when the angle between \a prevpt -> \a chkpt and \a chkpt -> candidate
     *  exceeds \c 0.4999*pi radians, so the search does not turn back on itself.
     *
     *  \param chkpt     The reference point.
     *  \param prevpt    The point preceding \a chkpt, which defines the travel direction.
     *  \param ptlist    The candidate points.
     *  \param exceptcnt Number of trailing entries in \a ptlist that are excluded from the search.
     *  \param outpt     Output that receives the nearest point when one is found. May be \c nullptr.
     *  \return The index of the nearest point in \a ptlist, or \c -1 if none qualifies.
     */
    int findNearestPointWithRad(const QPointF &chkpt, const QPointF &prevpt, const QList<QPointF> &ptlist,
                                int exceptcnt = 0, QPointF *outpt = nullptr);
    /*! \brief Attaches points of \a pstptlist to both ends of the curve being built in \a lnplot.
     *  \param curptlist  Points of the curve built in the current pass.
     *  \param pstptlist  Points visited in earlier passes, used as connection candidates.
     *  \param lnplot     The line plotter that receives the appended or prepended connection points.
     */
    void handleEnclosePoint(const QList<QPointF> &curptlist, const QList<QPointF> &pstptlist,
                            finPlotDotsLine *lnplot);
};

#endif // FINPLOTDOTS_H
