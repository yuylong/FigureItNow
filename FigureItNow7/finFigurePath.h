/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2025 Yulong Yu. All rights reserved.
 */

#ifndef FINFIGUREPATH_H
#define FINFIGUREPATH_H

#include <QPen>
#include <QBrush>
#include <QPainterPath>
#include <QPointF>
#include <QPixmap>
#include <QImage>

#include "finErrorCode.h"


class finFigurePath
{
protected:
    QPen _pen;
    QBrush _brush;
    QPainterPath _path;
    QPointF _imgPos;
    QPixmap _pixmap;

public:
    finFigurePath();
    finFigurePath(const finFigurePath &figpath);

    const QPen &getPen() const;
    const QBrush &getBrush() const;
    const QPainterPath &getPath() const;
    const QPointF &getImagePosition() const;
    const QPixmap &getPixmap() const;
    QImage getImage() const;

    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setPath(const QPainterPath &path);
    void setImagePosition(const QPointF &imgpos);
    void setPixmap(const QPixmap &pixmap);
    void setImage(const QImage &image);

    finFigurePath &operator = (const finFigurePath &figpath);
};

#endif // FINFIGUREPATH_H
