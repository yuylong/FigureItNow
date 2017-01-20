#ifndef FINGRAPHPANELPAINTER_H
#define FINGRAPHPANELPAINTER_H

#include "finGraphPanelBase.h"

#include <QPainter>

#include "finErrorCode.h"
#include "finFigureObject.h"
#include "finFigureContainer.h"


class finGraphPanelPainter : public finGraphPanelBase
{
protected:
    QPainter *_painter;

public:
    finGraphPanelPainter();

    QPainter *getPainter() const;
    virtual finErrorCode setPainter(QPainter *painter);

    virtual finErrorCode draw();

protected:
    virtual finErrorCode applyGraphConfig() const;
    virtual finErrorCode drawFigurePath(const finFigurePath &path) const;
};

#endif // FINGRAPHPANELPAINTER_H
