#ifndef FINFIGURECONTAINER_H
#define FINFIGURECONTAINER_H

#include <QList>

#include "finErrorCode.h"
#include "finFigureConfig.h"
#include "finFigureObject.h"

class finFigureContainer
{
protected:
    finFigureConfig _curFigCfg;
    QList<finFigureObject *> _figList;

public:
    finFigureContainer();

    const finFigureConfig *getFigureConfig() const;
    finFigureConfig *getFigureConfig();


};

#endif // FINFIGURECONTAINER_H
