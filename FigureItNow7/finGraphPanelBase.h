#ifndef FINGRAPHPANELBASE_H
#define FINGRAPHPANELBASE_H

#include <QPointF>
#include <QList>

#include "finErrorCode.h"
#include "finGraphConfig.h"
#include "finFigureObject.h"
#include "finFigurePath.h"
#include "finFigureContainer.h"

class finGraphPanelBase
{
protected:
    finGraphConfig _config;
    QList<finFigurePath> _pathList;

public:
    finGraphPanelBase();

    const finGraphConfig *getGraphConfig() const;
    finGraphConfig *getGraphConfig();
    finErrorCode setGraphConfig(const finGraphConfig *cfg);

    int getFigurePathCount() const;
    finFigurePath getFigurePathAt(int idx) const;
    finErrorCode clearFigurePathList();

    finErrorCode setFigureContainer(finFigureContainer *figcontainer);
    finErrorCode appendFigureObject(finFigureObject *obj);
    finErrorCode appendFigurePath(const finFigurePath &path);

    virtual finErrorCode draw();

protected:
    virtual finErrorCode applyGraphConfig() const;
    virtual finErrorCode drawFigurePath(const finFigurePath &path) const;
};

#endif // FINGRAPHPANELBASE_H
