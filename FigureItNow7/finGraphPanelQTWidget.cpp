#include "finGraphPanelQTWidget.h"

finGraphPanelQTWidget::finGraphPanelQTWidget()
{
    this->_widget = NULL;
    this->_paintEngine = NULL;
}

QPaintDevice *finGraphPanelQTWidget::getWidget() const
{
    return this->_widget;
}

finErrorCode finGraphPanelQTWidget::setWidget(QPaintDevice *widget)
{
    this->releasePaintEngine();
    this->_widget = widget;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

QPaintEngine *finGraphPanelQTWidget::getPaintEngine()
{
    if ( this->_widget == NULL ) {
        this->_paintEngine = NULL;
        return NULL;
    }

    if ( this->_paintEngine == NULL ) {
        this->_paintEngine = this->_widget->paintEngine();
        this->_paintEngine->begin(this->_widget);
    }

    return this->_paintEngine;
}

void finGraphPanelQTWidget::releasePaintEngine()
{
    this->_paintEngine->end();
    delete this->_paintEngine;
    this->_paintEngine = NULL;
}

finErrorCode finGraphPanelQTWidget::initDrawRes()
{
    QPaintEngine *pe = this->getPaintEngine();
    if ( pe == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;
    else
        return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finGraphPanelQTWidget::disposeDrawRes()
{
    this->releasePaintEngine();
}

finErrorCode finGraphPanelQTWidget::drawLine(finFigureObjectLine *line)
{
    QPaintEngine *pe = this->getPaintEngine();
    if ( pe == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    QLineF qline;
    qline.setP1(line->getPoint1());
    qline.setP2(line->getPoint1());
    pe->drawLines(&qline, 1);

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}
