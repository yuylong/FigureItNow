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

protected:
    void clearScene();

    finErrorCode drawObjLine(finFigureObjectLine *line);

    finErrorCode drawObjLine3D(finFigureObjectLine3D *line3d);

};

#endif // FINGRAPHPANELSCENE_H
