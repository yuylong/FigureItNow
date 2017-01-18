#include "finFigurePath.h"

finFigurePath::finFigurePath()
    : _pen(Qt::black, 1), _brush(Qt::transparent), _path()
{
    /* Do Nothing */
}

finFigurePath::finFigurePath(const finFigurePath &figpath)
    : _pen(figpath._pen), _brush(figpath._brush), _path(figpath._path)
{
    /* Do Nothing */
}

QPen finFigurePath::getPen() const
{
    return this->_pen;
}

QBrush finFigurePath::getBrush() const
{
    return this->_brush;
}

QPainterPath finFigurePath::getPath() const
{
    return this->_path;
}

finErrorCode finFigurePath::setPen(const QPen &pen)
{
    this->_pen = pen;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigurePath::setBrush(const QBrush &brush)
{
    this->_brush = brush;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigurePath::setPath(const QPainterPath &path)
{
    this->_path = path;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finFigurePath &finFigurePath::operator = (const finFigurePath &figpath)
{
    this->_pen = figpath._pen;
    this->_brush = figpath._brush;
    this->_path = figpath._path;
    return *this;
}