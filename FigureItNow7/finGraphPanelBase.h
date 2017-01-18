#ifndef FINGRAPHPANELBASE_H
#define FINGRAPHPANELBASE_H

#include <QPointF>

#include "finErrorCode.h"
#include "finGraphConfig.h"
#include "finFigureObject.h"
#include "finFigurePath.h"
#include "finFigureContainer.h"

class finGraphPanelBase
{
protected:
    finGraphConfig _config;

public:
    finGraphPanelBase();

    const finGraphConfig *getGraphConfig() const;
    finGraphConfig *getGraphConfig();
    finErrorCode setGraphConfig(const finGraphConfig *cfg);

    virtual finErrorCode applyGraphConfig();
    virtual finErrorCode drawContainer(finFigureContainer *figcontainer);
    virtual finErrorCode drawObject(finFigureObject *obj);
    virtual finErrorCode drawFigurePath(const finFigurePath &path);
};

#endif // FINGRAPHPANELBASE_H
