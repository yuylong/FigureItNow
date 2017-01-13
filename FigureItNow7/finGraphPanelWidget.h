#ifndef FINGRAPHPANELWIDGET_H
#define FINGRAPHPANELWIDGET_H

#include "finGraphPanelBase.h"

#include <QPaintDevice>

#include "finErrorCode.h"
#include "finFigureObject.h"
#include "finFigureContainer.h"


class finGraphPanelWidget : public finGraphPanelBase
{
protected:
    QPaintDevice *_widget;

public:
    finGraphPanelWidget();

    QPaintDevice *getWidget() const;
    finErrorCode setWidget(QPaintDevice *widget);

    virtual finErrorCode drawContainer(finFigureContainer *figcontainer);
    virtual finErrorCode drawObject(finFigureObject *obj);
};

#endif // FINGRAPHPANELWIDGET_H
