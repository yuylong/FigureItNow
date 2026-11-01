/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finGraphPanelPainter.h
 *  \brief Declaration of the graph panel that paints into an externally supplied QPainter.
 *
 *  This panel implements the painting hooks of finGraphPanelBase with plain QPainter calls, so it
 *  can target any paint device the caller already created a painter for.
 */

#ifndef FINGRAPHPANELPAINTER_H
#define FINGRAPHPANELPAINTER_H

#include "finGraphPanelBase.h"

#include <QPainter>

#include "finErrorCode.h"
#include "finFigureObject.h"
#include "finFigureContainer.h"


/*! \class finGraphPanelPainter
 *  \brief Paints the queued pixel paths through a caller-provided QPainter.
 *
 *  The class fills in the two rendering hooks left empty by finGraphPanelBase: applyGraphConfig()
 *  clears the panel rectangle with the configured background and installs the render hints, and
 *  drawFigurePath() draws one path with its own pen and brush. The painter is borrowed from the
 *  caller and never deleted here, so the caller keeps it alive for the whole draw() call.
 *  finGraphPanelWidget builds on this class to draw into a QPaintDevice.
 */
class finGraphPanelPainter : public finGraphPanelBase
{
protected:
    QPainter *_painter;             //!< Borrowed painter; nullptr when none is installed.

public:
    /*! \brief Constructs a panel without an installed painter. */
    finGraphPanelPainter();

    /*! \name Painter Attachment
     *  \brief Attach or inspect the borrowed target painter.
     */
    ///@{
    /*! \brief Returns the installed painter, or nullptr when none is installed. */
    QPainter *getPainter() const;
    /*! \brief Installs \a painter as the drawing target; pass nullptr to detach it.
     *
     *  The panel keeps only the pointer, so \a painter must stay alive until it is detached.
     *  \return Always EC_SUCCESS.
     */
    virtual finErrorCode setPainter(QPainter *painter);
    ///@}

    /*! \brief Draws all queued paths with the installed painter.
     *
     *  \return EC_STATE_ERROR if no painter is installed, otherwise the result summarizing the
     *          drawing of every path as reported by finGraphPanelBase::draw().
     */
    virtual finErrorCode draw();

protected:
    /*! \brief Clears the panel rectangle with the configured background and installs the render
     *         hints of the graph configuration.
     */
    virtual finErrorCode applyGraphConfig() const;
    /*! \brief Draws one pixel path, including its optional raster payload, with its own pen and
     *         brush.
     */
    virtual finErrorCode drawFigurePath(const finFigurePath &path) const;
};

#endif // FINGRAPHPANELPAINTER_H
