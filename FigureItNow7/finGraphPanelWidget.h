/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finGraphPanelWidget.h
 *  \brief Declaration of the graph panel that paints itself onto a QPaintDevice.
 *
 *  This panel creates its own QPainter on a borrowed paint device for the duration of one draw
 *  pass, so it can render into a widget, an image, a printer or any other QPaintDevice without the
 *  caller having to manage a painter.
 */

#ifndef FINGRAPHPANELWIDGET_H
#define FINGRAPHPANELWIDGET_H

#include "finGraphPanelPainter.h"

#include <QPaintDevice>

#include "finErrorCode.h"


/*! \class finGraphPanelWidget
 *  \brief Paints the queued pixel paths onto a borrowed QPaintDevice.
 *
 *  The class borrows a paint device and creates a local QPainter on it inside draw(), which is then
 *  bound to the inherited _painter field only for that call. Because the painter lives on the stack
 *  of draw(), the caller never manages it and setPainter() is not usable; only setWidget() selects
 *  the drawing target. The device is not owned, so it must outlive every draw() call.
 *
 *  Although the accessors are named after widgets, the target is a QPaintDevice and is used with
 *  QImage, QPdfWriter, QSvgGenerator and QPrinter as well as with real widgets.
 *
 *  \note The inherited _painter is nullptr outside draw(); reading it before the first draw, or
 *        after one has finished, does not report an error but a detached state.
 */
class finGraphPanelWidget : public finGraphPanelPainter
{
protected:
    QPaintDevice *_widget;          //!< Borrowed paint device; nullptr when none is installed.

public:
    /*! \brief Constructs a panel without an installed paint device. */
    finGraphPanelWidget();

    /*! \brief Rejects painter attachment, since the painter is created internally by draw().
     *
     *  \return Always EC_INVALID_PARAM, regardless of \a painter.
     */
    virtual finErrorCode setPainter(QPainter *painter);

    /*! \name Paint Device Attachment
     *  \brief Attach or inspect the borrowed drawing target.
     */
    ///@{
    /*! \brief Returns the installed paint device, or nullptr when none is installed. */
    QPaintDevice *getWidget() const;
    /*! \brief Installs \a widget as the drawing target; pass nullptr to detach it.
     *
     *  The panel keeps only the pointer and does not delete it.
     */
    void setWidget(QPaintDevice *widget);
    ///@}

    /*! \brief Draws all queued paths onto the installed paint device.
     *
     *  Creates a QPainter on the device, binds it for the duration of the call, delegates to
     *  finGraphPanelPainter::draw(), and then unbinds it again.
     *
     *  \return The result reported by finGraphPanelPainter::draw().
     *  \exception finException EC_STATE_ERROR when no paint device is installed.
     */
    virtual finErrorCode draw();

protected:
    /*! \brief Applies the graph configuration through the painter bound by draw().
     *
     *  \exception finException EC_STATE_ERROR when no paint device is installed.
     */
    virtual finErrorCode applyGraphConfig() const;
    /*! \brief Draws one pixel path through the painter bound by draw().
     *
     *  \exception finException EC_STATE_ERROR when no paint device is installed.
     */
    virtual finErrorCode drawFigurePath(const finFigurePath &path) const;
};

#endif // FINGRAPHPANELWIDGET_H
