#ifndef FINFIGURECONTAINER_H
#define FINFIGURECONTAINER_H

#include <QList>

#include "finErrorCode.h"
#include "finFigureConfig.h"
#include "finGraphConfig.h"
#include "finFigureObject.h"

class finFigureContainer
{
protected:
    finFigureConfig _curFigCfg;
    finGraphConfig _graphCfg;
    QList<finFigureObject *> _figList;

public:
    finFigureContainer();

    const finFigureConfig *getFigureConfig() const;
    finFigureConfig *getFigureConfig();
    const finGraphConfig *getGraphConfig() const;
    finGraphConfig *getGraphConfig();
    finErrorCode setFigureConfigForObject(finFigureObject *figobj) const;

    int getFigureObjectCount() const;
    finFigureObject *getFigureObjectAt(int idx);

    finErrorCode appendFigureObject(finFigureObject *figobj);
};

#endif // FINFIGURECONTAINER_H
