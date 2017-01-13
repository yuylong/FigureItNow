#include "finGraphPanelWidget.h"

#include "finGraphPanelPainter.h"


finGraphPanelWidget::finGraphPanelWidget()
{
    this->_widget = NULL;
}

QPaintDevice *finGraphPanelWidget::getWidget() const
{
    return this->_widget;
}

finErrorCode finGraphPanelWidget::setWidget(QPaintDevice *widget)
{
    this->_widget = widget;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphPanelWidget::applyGraphConfig()
{
    if ( this->_widget == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    QPainter painter(this->_widget);
    finGraphPanelPainter fgpp;
    fgpp.setPainter(&painter);

    finErrorCode errcode = fgpp.applyGraphConfig();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphPanelWidget::drawContainer(finFigureContainer *figcontainer)
{
    if ( this->_widget == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    QPainter painter(this->_widget);
    finGraphPanelPainter fgpp;
    fgpp.setPainter(&painter);

    finErrorCode errcode = fgpp.drawContainer(figcontainer);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphPanelWidget::drawObject(finFigureObject *obj)
{
    if ( this->_widget == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    QPainter painter(this->_widget);
    finGraphPanelPainter fgpp;
    fgpp.setPainter(&painter);

    finErrorCode errcode = fgpp.drawObject(obj);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}
