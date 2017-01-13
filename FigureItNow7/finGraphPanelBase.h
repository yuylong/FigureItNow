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

    virtual finErrorCode drawContainer(finFigureContainer *figcontainer);
    virtual finErrorCode drawObject(finFigureObject *obj);
};

#endif // FINGRAPHPANELBASE_H
