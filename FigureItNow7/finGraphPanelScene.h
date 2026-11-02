/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finGraphPanelScene.h
 *  \brief Declaration of the graph panel that renders into a QGraphicsScene.
 *
 *  This panel implements the painting hooks of finGraphPanelBase by adding items to an externally
 *  owned QGraphicsScene, so the figure can be shown by any QGraphicsView attached to that scene.
 */

#ifndef FINGRAPHPANELSCENE_H
#define FINGRAPHPANELSCENE_H

#include "finGraphPanelBase.h"

#include <QGraphicsScene>

#include "finErrorCode.h"


/*! \class finGraphPanelScene
 *  \brief Renders the queued pixel paths as items of a caller-provided QGraphicsScene.
 *
 *  The class fills in the two rendering hooks left empty by finGraphPanelBase: applyGraphConfig()
 *  empties the scene and applies the background, scene rectangle and render hints of the graph
 *  configuration, while drawFigurePath() adds one vector path item and, when present, a positioned
 *  pixmap item. The scene is borrowed from the caller and never deleted here.
 *
 *  \note Every draw() starts from an empty scene, so all items present at that moment are removed
 *        and deleted. Do not share the target scene with content that must survive a redraw.
 */
class finGraphPanelScene : public finGraphPanelBase
{
protected:
    QGraphicsScene *_scene;         //!< Borrowed target scene; nullptr when none is installed.

public:
    /*! \brief Constructs a panel without an installed scene. */
    finGraphPanelScene();

    /*! \name Scene Attachment
     *  \brief Attach or inspect the borrowed target scene.
     */
    ///@{
    /*! \brief Returns the installed scene, or nullptr when none is installed. */
    QGraphicsScene *getScene() const;
    /*! \brief Installs \a scene as the drawing target; pass nullptr to detach it.
     *
     *  The panel keeps only the pointer, so \a scene must outlive its use by this panel.
     *  \return Always EC_SUCCESS.
     */
    finErrorCode setScene(QGraphicsScene *scene);
    ///@}

    /*! \brief Draws all queued paths as items of the installed scene.
     *
     *  \return EC_STATE_ERROR if no scene is installed, otherwise the result summarizing the
     *          drawing of every path as reported by finGraphPanelBase::draw().
     */
    virtual finErrorCode draw();

protected:
    /*! \brief Empties the scene and applies the background, scene rectangle and render hints of
     *         the graph configuration.
     */
    virtual finErrorCode applyGraphConfig() const;
    /*! \brief Adds one pixel path, and its optional raster payload, as items of the scene. */
    virtual finErrorCode drawFigurePath(const finFigurePath &path) const;

    /*! \brief Removes and deletes every item currently contained in the scene. */
    void clearScene() const;
};

#endif // FINGRAPHPANELSCENE_H
