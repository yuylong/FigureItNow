/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#ifndef FINGRAPHPANELSCENE_H
#define FINGRAPHPANELSCENE_H

#include "finGraphPanelBase.h"

#include <QGraphicsScene>

#include "finErrorCode.h"


class finGraphPanelScene : public finGraphPanelBase
{
protected:
    QGraphicsScene *_scene;

public:
    finGraphPanelScene();

    QGraphicsScene *getScene() const;
    finErrorCode setScene(QGraphicsScene *scene);

    virtual finErrorCode draw();

protected:
    virtual finErrorCode applyGraphConfig() const;
    virtual finErrorCode drawFigurePath(const finFigurePath &path) const;

    void clearScene() const;
};

#endif // FINGRAPHPANELSCENE_H
