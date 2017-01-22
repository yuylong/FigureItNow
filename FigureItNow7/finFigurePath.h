#ifndef FINFIGUREPATH_H
#define FINFIGUREPATH_H

#include <QPen>
#include <QBrush>
#include <QPainterPath>
#include <QImage>

#include "finErrorCode.h"

class finFigurePath
{
protected:
    QPen _pen;
    QBrush _brush;

    QPainterPath _path;
    QImage _img;

public:
    finFigurePath();
    finFigurePath(const finFigurePath &figpath);

    QPen getPen() const;
    QBrush getBrush() const;
    QPainterPath getPath() const;
    QImage getImage() const;

    finErrorCode setPen(const QPen &pen);
    finErrorCode setBrush(const QBrush &brush);
    finErrorCode setPath(const QPainterPath &path);
    finErrorCode setImage(const QImage &image);

    finFigurePath &operator = (const finFigurePath &figpath);
};

#endif // FINFIGUREPATH_H
