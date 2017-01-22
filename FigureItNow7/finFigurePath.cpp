#include "finFigurePath.h"

finFigurePath::finFigurePath()
    : _pen(Qt::black, 1), _brush(Qt::transparent), _path(),
      _imgPos(0.0, 0.0), _pixmap()
{
    /* Do Nothing */
}

finFigurePath::finFigurePath(const finFigurePath &figpath)
    : _pen(figpath._pen), _brush(figpath._brush), _path(figpath._path),
      _imgPos(figpath._imgPos), _pixmap(figpath._pixmap)
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

QPointF finFigurePath::getImagePosition() const
{
    return this->_imgPos;
}

QPixmap finFigurePath::getPixmap() const
{
    return this->_pixmap;
}

QImage finFigurePath::getImage() const
{
    return this->_pixmap.toImage();
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

finErrorCode finFigurePath::setImagePosition(const QPointF &imgpos)
{
    this->_imgPos = imgpos;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigurePath::setPixmap(const QPixmap &pixmap)
{
    this->_pixmap = pixmap;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigurePath::setImage(const QImage &image)
{
    this->_pixmap = QPixmap::fromImage(image);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finFigurePath &finFigurePath::operator = (const finFigurePath &figpath)
{
    this->_pen = figpath._pen;
    this->_brush = figpath._brush;
    this->_path = figpath._path;
    this->_imgPos = figpath._imgPos;
    this->_pixmap = figpath._pixmap;
    return *this;
}
