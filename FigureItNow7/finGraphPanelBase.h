#ifndef FINGRAPHPANELBASE_H
#define FINGRAPHPANELBASE_H

#include <QPointF>

#include "finErrorCode.h"
#include "finGraphConfig.h"
#include "finFigureObject.h"
#include "finFigureContainer.h"

class finGraphPanelBase
{
protected:
    finGraphConfig _config;

public:
    finGraphPanelBase();

    const finGraphConfig *getGraphConfig() const;
    finGraphConfig *getGraphConfig();

    finErrorCode drawContainer(finFigureContainer *figcontainer);
    finErrorCode drawOneObject(finFigureObject *obj);

    virtual finErrorCode drawLine(finFigureObjectLine *line) = 0;
};

#endif // FINGRAPHPANELBASE_H
