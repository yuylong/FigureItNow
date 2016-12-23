#ifndef FINGRAPHPANELBASE_H
#define FINGRAPHPANELBASE_H

#include <QPointF>

#include "finErrorCode.h"
#include "finGraphConfig.h"

class finGraphPanelBase
{
protected:
    finGraphConfig _config;

public:
    finGraphPanelBase();

    virtual finErrorCode drawLine(const QPointF &pt1, const QPointF &pt2) = 0;
};

#endif // FINGRAPHPANELBASE_H
