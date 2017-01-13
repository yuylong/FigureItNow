#include "finUiFigureWidget.h"

#include <QPaintEvent>

#include "finGraphPanelWidget.h"


finUiFigureWidget::finUiFigureWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    /* Do Nothing */
}

const finFigureContainer *finUiFigureWidget::getFigureContainer() const
{
    return &this->_figcontainer;
}

finFigureContainer *finUiFigureWidget::getFigureContainer()
{
    return &this->_figcontainer;
}

void finUiFigureWidget::paintEvent(QPaintEvent *e)
{
    finGraphPanelWidget gp;
    gp.setWidget(this);
    gp.setGraphConfig(this->_figcontainer.getGraphConfig());
    gp.drawContainer(&this->_figcontainer);

    e->accept();
}
