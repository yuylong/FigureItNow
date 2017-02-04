#ifndef FINPLOTDOTS_H
#define FINPLOTDOTS_H

#include <QList>
#include <QPointF>

#include "finFigureObject.h"
#include "finFigureContainer.h"

class finPlotDots
{
protected:
    QList<QPointF> _ptList;
    finFigureContainer *_figcontainer;

public:
    finPlotDots();

    int getPointCount() const;
    const QPointF &getPointAt(int idx) const;
    finFigureContainer *getFigureContainer() const;

    finErrorCode appendPoint(const QPointF &pt);
    finErrorCode clearPoints();
    finErrorCode setFigureContainer(finFigureContainer *figcontainer);

    virtual finErrorCode plot();
};

#endif // FINPLOTDOTS_H
