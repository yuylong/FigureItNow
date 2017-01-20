#ifndef FINGRAPHPANELWIDGET_H
#define FINGRAPHPANELWIDGET_H

#include "finGraphPanelBase.h"
#include "finGraphPanelPainter.h"

#include <QPaintDevice>

#include "finErrorCode.h"
#include "finFigureObject.h"
#include "finFigureContainer.h"


class finGraphPanelWidget : public finGraphPanelPainter
{
protected:
    QPaintDevice *_widget;

public:
    finGraphPanelWidget();

    virtual finErrorCode setPainter(QPainter *painter);

    QPaintDevice *getWidget() const;
    finErrorCode setWidget(QPaintDevice *widget);

    virtual finErrorCode draw();

protected:
    virtual finErrorCode applyGraphConfig() const;
    virtual finErrorCode drawFigurePath(const finFigurePath &path) const;
};

#endif // FINGRAPHPANELWIDGET_H
