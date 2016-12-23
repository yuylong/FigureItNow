#ifndef FINFIGURECONTAINER_H
#define FINFIGURECONTAINER_H

#include <QList>

#include "finFigureObject.h"

class finFigureContainer
{
protected:
    QList<finFigureObject *> _figList;

public:
    finFigureContainer();
};

#endif // FINFIGURECONTAINER_H
