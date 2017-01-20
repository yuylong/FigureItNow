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
