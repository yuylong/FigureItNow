#include "finGraphConfig.h"

finGraphConfig::finGraphConfig()
{
    this->_panelWidth = 640;
    this->_panelHeight = 480;
    this->_startX = -20.0;
    this->_endX = 20.0;
    this->_startY = -15.0;
    this->_endY = 15.0;
    this->_bgColor[finFigureConfig::FIN_CI_RED] = 1.0;
    this->_bgColor[finFigureConfig::FIN_CI_GREEN] = 1.0;
    this->_bgColor[finFigureConfig::FIN_CI_BLUE] = 1.0;
    this->_bgColor[finFigureConfig::FIN_CI_TRANSPARENCY] = 0.0;
}

