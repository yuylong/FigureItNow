/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 2nd, 2017
 * Copyright(c) 2017-2026 Yulong Yu. All rights reserved.
 */

/*! \file finFigureContainer.h
 *  \brief Declaration of the figure collection and its shared drawing configuration.
 *
 *  A figure container owns the generated finFigureObject instances for one execution result and
 *  carries the graph configuration and current figure style used when new objects are appended.
 */

#ifndef FINFIGURECONTAINER_H
#define FINFIGURECONTAINER_H

#include <QList>

#include "finErrorCode.h"
#include "finFigureConfig.h"
#include "finGraphConfig.h"
#include "finFigureObject.h"


/*! \class finFigureContainer
 *  \brief Owns a collection of figure objects and their shared drawing context.
 *
 *  Newly appended objects receive a copy of the current finFigureConfig. The container owns every
 *  appended finFigureObject and deletes it when cleared or destroyed.
 */
class finFigureContainer
{
protected:
    finFigureConfig _curFigCfg;        //!< Style copied to each figure as it is appended.
    finGraphConfig _graphCfg;          //!< Coordinate and panel configuration for the figure collection.
    QList<finFigureObject *> _figList; //!< Owned figure objects in rendering order.

public:
    /*! \brief Constructs an empty container initialized with the default figure style. */
    finFigureContainer();
    /*! \brief Deletes all figure objects still owned by the container. */
    ~finFigureContainer();

    /*! \name Shared Configuration
     *  \brief Access the current figure style and graph coordinate configuration.
     */
    ///@{
    /*! \brief Returns the immutable current figure style. */
    const finFigureConfig *getFigureConfig() const;
    /*! \brief Returns the mutable current figure style. */
    finFigureConfig *getFigureConfig();
    /*! \brief Returns the immutable graph configuration. */
    const finGraphConfig *getGraphConfig() const;
    /*! \brief Returns the mutable graph configuration. */
    finGraphConfig *getGraphConfig();
    /*! \brief Copies the current figure style to \a figobj. */
    void setFigureConfigForObject(finFigureObject *figobj) const;
    ///@}

    /*! \name Figure Object Collection
     *  \brief Inspect, append, and release the owned figures in rendering order.
     */
    ///@{
    /*! \brief Returns the number of figure objects currently owned. */
    int getFigureObjectCount() const;
    /*! \brief Returns the figure object at \a idx. */
    finFigureObject *getFigureObjectAt(int idx);

    /*! \brief Copies the current style to and takes ownership of \a figobj. */
    void appendFigureObject(finFigureObject *figobj);

    /*! \brief Deletes all owned figure objects and empties the collection. */
    void clearFigureObjects();
    ///@}

    /*! \brief Writes diagnostic information for all contained figures to standard output. */
    void dump() const;
};

#endif // FINFIGURECONTAINER_H
