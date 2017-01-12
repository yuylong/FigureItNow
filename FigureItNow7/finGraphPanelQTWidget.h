#ifndef FINGRAPHPANELQTWIDGET_H
#define FINGRAPHPANELQTWIDGET_H

#include "finGraphPanelBase.h"

class finGraphPanelQTWidget : public finGraphPanelBase
{
protected:


public:
    finGraphPanelQTWidget();

    virtual finErrorCode drawLine(finFigureObjectLine *line);
};

#endif // FINGRAPHPANELQTWIDGET_H
