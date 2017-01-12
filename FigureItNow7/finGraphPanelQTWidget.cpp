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

QPainter *finGraphPanelQTWidget::getPaintEngine()
{
    if ( this->_widget == NULL ) {
        this->_painter = NULL;
        return NULL;
    }

    if ( this->_painter == NULL ) {
        this->_painter = new QPainter(this->_widget);
        //this->_paintEngine->begin(this->_widget);
    }
    return this->_painter;
}

void finGraphPanelQTWidget::releasePaintEngine()
{
    if ( this->_painter != NULL ) {
        //this->_paintEngine->end();
        //this->_painter->endNativePainting();
        //this->_painter->end();
        //delete this->_painter;
    }
    this->_painter = NULL;
}

finErrorCode finGraphPanelQTWidget::initDrawRes()
{
    //QPainter *pe = this->getPaintEngine();
    //if ( pe == NULL )
    //    return finErrorCodeKits::FIN_EC_STATE_ERROR;
    //else
        return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finGraphPanelQTWidget::disposeDrawRes()
{
    //this->releasePaintEngine();
}

finErrorCode finGraphPanelQTWidget::drawLine(finFigureObjectLine *line)
{
    QPainter qp(this->_widget);
    //if ( qp == NULL )
    //    return finErrorCodeKits::FIN_EC_STATE_ERROR;

    qp.drawLine(line->getPoint1(), line->getPoint2());
    return finErrorCodeKits::FIN_EC_SUCCESS;
}
