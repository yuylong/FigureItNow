/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2025 Yulong Yu. All rights reserved.
 */

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

const QPen &finFigurePath::getPen() const
{
    return this->_pen;
}

const QBrush &finFigurePath::getBrush() const
{
    return this->_brush;
}

const QPainterPath &finFigurePath::getPath() const
{
    return this->_path;
}

const QPointF &finFigurePath::getImagePosition() const
{
    return this->_imgPos;
}

const QPixmap &finFigurePath::getPixmap() const
{
    return this->_pixmap;
}

QImage finFigurePath::getImage() const
{
    return this->_pixmap.toImage();
}

void finFigurePath::setPen(const QPen &pen)
{
    this->_pen = pen;
}

void finFigurePath::setBrush(const QBrush &brush)
{
    this->_brush = brush;
}

void finFigurePath::setPath(const QPainterPath &path)
{
    this->_path = path;
}

void finFigurePath::setImagePosition(const QPointF &imgpos)
{
    this->_imgPos = imgpos;
}

void finFigurePath::setPixmap(const QPixmap &pixmap)
{
    this->_pixmap = pixmap;
}

void finFigurePath::setImage(const QImage &image)
{
    this->_pixmap = QPixmap::fromImage(image);
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
