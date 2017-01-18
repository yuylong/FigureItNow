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

    virtual finErrorCode applyGraphConfig();
    virtual finErrorCode drawContainer(finFigureContainer *figcontainer);
    virtual finErrorCode drawObject(finFigureObject *obj);
    virtual finErrorCode drawFigurePath(const finFigurePath &path);

protected:
    void clearScene();
};

#endif // FINGRAPHPANELSCENE_H
