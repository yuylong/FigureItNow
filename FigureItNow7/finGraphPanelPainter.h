#ifndef FINGRAPHPANELPAINTER_H
#define FINGRAPHPANELPAINTER_H

#include "finGraphPanelBase.h"

#include <QPainter>

#include "finErrorCode.h"
#include "finFigureObject.h"
#include "finFigureContainer.h"


class finGraphPanelPainter : public finGraphPanelBase
{
protected:
    QPainter *_painter;

public:
    finGraphPanelPainter();

    QPainter *getPainter() const;
    finErrorCode setPainter(QPainter *painter);

    virtual finErrorCode applyGraphConfig();
    virtual finErrorCode drawContainer(finFigureContainer *figcontainer);
    virtual finErrorCode drawObject(finFigureObject *obj);

protected:
    finErrorCode applyFigureConfig(finFigureConfig *cfg);
    finErrorCode drawObjPath(finFigureObject *obj);

    finErrorCode drawObjLine3D(finFigureObjectLine3D *line3d);
};

#endif // FINGRAPHPANELPAINTER_H
