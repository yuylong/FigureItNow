#ifndef FINUIFIGUREWIDGET_H
#define FINUIFIGUREWIDGET_H

#include <QWidget>

#include "finErrorCode.h"
#include "finFigureContainer.h"
#include "finGraphPanelWidget.h"


class finUiFigureWidget : public QWidget
{
protected:
    finFigureContainer _figcontainer;
    finGraphPanelWidget _graphpanel;

public:
    finUiFigureWidget(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

    const finFigureContainer *getFigureContainer() const;
    finFigureContainer *getFigureContainer();
    const finGraphPanelWidget *getGraphPanel() const;
    finGraphPanelWidget *getGraphPanel();

    finErrorCode applyFigure();

protected:
    virtual void paintEvent(QPaintEvent *e);
};

#endif // FINUIFIGUREWIDGET_H
