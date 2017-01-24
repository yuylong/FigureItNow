#include "finUiFigureWidget.h"

#include <QPaintEvent>


finUiFigureWidget::finUiFigureWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f), _figcontainer(), _graphpanel()
{
    this->_graphpanel.setWidget(this);
}

const finFigureContainer *finUiFigureWidget::getFigureContainer() const
{
    return &this->_figcontainer;
}

finFigureContainer *finUiFigureWidget::getFigureContainer()
{
    return &this->_figcontainer;
}

const finGraphPanelWidget *finUiFigureWidget::getGraphPanel() const
{
    return &this->_graphpanel;
}

finGraphPanelWidget *finUiFigureWidget::getGraphPanel()
{
    return &this->_graphpanel;
}

finErrorCode finUiFigureWidget::applyFigure()
{
    QRect georect = this->geometry();
    georect.setSize(this->_figcontainer.getGraphConfig()->getPanelPixelSize().toSize());
    this->setGeometry(georect);

    this->_graphpanel.setFigureContainer(&this->_figcontainer);
    this->repaint();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finUiFigureWidget::paintEvent(QPaintEvent *e)
{
    this->_graphpanel.draw();
    e->accept();
}
