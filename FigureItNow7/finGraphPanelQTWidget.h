#ifndef FINGRAPHPANELQTWIDGET_H
#define FINGRAPHPANELQTWIDGET_H

#include <QPaintDevice>
#include <QPaintEngine>

#include "finErrorCode.h"
#include "finGraphPanelBase.h"

class finGraphPanelQTWidget : public finGraphPanelBase
{
protected:
    QPaintDevice *_widget;
    QPaintEngine *_paintEngine;

public:
    finGraphPanelQTWidget();

    QPaintDevice *getWidget() const;
    finErrorCode setWidget(QPaintDevice *widget);

    virtual finErrorCode initDrawRes();
    virtual void disposeDrawRes();

protected:
    QPaintEngine *getPaintEngine();
    void releasePaintEngine();


    virtual finErrorCode drawLine(finFigureObjectLine *line);
};

#endif // FINGRAPHPANELQTWIDGET_H
