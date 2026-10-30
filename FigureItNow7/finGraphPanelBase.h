/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finGraphPanelBase.h
 *  \brief Declaration of the abstract base class of the graph panels.
 *
 *  A graph panel accepts a finFigureContainer, converts its figure objects into pixel-space
 *  finFigurePath payloads, and paints those payloads onto a drawing surface. The concrete surface
 *  and the actual painting operations are supplied by the derived classes.
 */

#ifndef FINGRAPHPANELBASE_H
#define FINGRAPHPANELBASE_H

#include <QPointF>
#include <QList>

#include "finErrorCode.h"
#include "finGraphConfig.h"
#include "finFigureObject.h"
#include "finFigurePath.h"
#include "finFigureContainer.h"


/*! \class finGraphPanelBase
 *  \brief Converts figures into pixel paths and paints them onto a derived-class surface.
 *
 *  The base class owns a finGraphConfig describing the drawing panel and a list of pixel-space
 *  finFigurePath payloads produced from the appended figure objects. setFigureContainer() performs
 *  the conversion in one step, while appendFigureObject() and appendFigurePath() add content
 *  incrementally. draw() then calls applyGraphConfig() once and drawFigurePath() for every stored
 *  path. Both of those hooks are no-ops in the base class, so a subclass must override them to
 *  render anything, while the path list handling is shared among all panels.
 */
class finGraphPanelBase
{
protected:
    finGraphConfig _config;         //!< Panel and coordinate mapping used to convert figure objects.
    QList<finFigurePath> _pathList; //!< Pixel-space paths to be painted, in rendering order.

public:
    /*! \brief Constructs a panel with the default graph configuration and an empty path list. */
    finGraphPanelBase();

    /*! \name Panel Configuration
     *  \brief Access the graph configuration shared by the panel and the figures it converts.
     */
    ///@{
    /*! \brief Returns the immutable graph configuration of the panel. */
    const finGraphConfig *getGraphConfig() const;
    /*! \brief Returns the mutable graph configuration of the panel. */
    finGraphConfig *getGraphConfig();
    /*! \brief Copies all settings of \a cfg into the panel; \a cfg must not be nullptr. */
    finErrorCode setGraphConfig(const finGraphConfig *cfg);
    ///@}

    /*! \name Figure Path Collection
     *  \brief Inspect and reset the pixel-space paths queued for painting.
     */
    ///@{
    /*! \brief Returns the number of pixel paths queued for painting. */
    int getFigurePathCount() const;
    /*! \brief Returns the path at \a idx, or an empty path when \a idx is out of range. */
    finFigurePath getFigurePathAt(int idx) const;
    /*! \brief Removes all queued paths. */
    finErrorCode clearFigurePathList();
    ///@}

    /*! \name Figure Content
     *  \brief Convert figures into pixel paths and append them to the panel.
     */
    ///@{
    /*! \brief Replaces the panel content by the figures in \a figcontainer and copies its graph
     *         configuration.
     *
     *  \return EC_NULL_POINTER if \a figcontainer is nullptr, EC_REACH_BOTTOM if it holds no
     *          figure, EC_READ_ERROR if none of its figures could be converted, EC_SUCCESS if all
     *          of them were converted, or EC_NORMAL_WARN if only part of them were.
     */
    finErrorCode setFigureContainer(finFigureContainer *figcontainer);
    /*! \brief Converts \a obj into pixel paths and appends them to the panel.
     *
     *  \return EC_NULL_POINTER if \a obj is nullptr, otherwise the result reported by the figure.
     */
    finErrorCode appendFigureObject(finFigureObject *obj);
    /*! \brief Appends an already converted pixel path. */
    finErrorCode appendFigurePath(const finFigurePath &path);
    ///@}

    /*! \brief Paints all queued paths onto the drawing surface of the derived class.
     *
     *  Applies the graph configuration first and then draws every queued path through
     *  drawFigurePath(). The return value summarizes the outcome: EC_REACH_BOTTOM when nothing is
     *  queued, EC_READ_ERROR when every path failed, EC_SUCCESS when every path was drawn, and
     *  EC_NORMAL_WARN when only part of the paths were drawn.
     */
    virtual finErrorCode draw();

protected:
    /*! \brief Applies the graph configuration to the drawing surface; does nothing in the base
     *         class.
     */
    virtual finErrorCode applyGraphConfig() const;
    /*! \brief Paints one pixel path; returns EC_NON_IMPLEMENT in the base class. */
    virtual finErrorCode drawFigurePath(const finFigurePath &path) const;
};

#endif // FINGRAPHPANELBASE_H
