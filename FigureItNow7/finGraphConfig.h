#ifndef FINGRAPHCONFIG_H
#define FINGRAPHCONFIG_H

#include <QRectF>

#include "finErrorCode.h"
#include "finFigureConfig.h"

class finGraphConfig
{
protected:
    unsigned long _panelWidth;
    unsigned long _panelHeight;

    double _startX, _endX;
    double _startY, _endY;

    double _bgColor[finFigureConfig::FIN_CI_MAX];

public:
    finGraphConfig();
};

#endif // FINGRAPHCONFIG_H
