#ifndef FINFIGUREPATH_H
#define FINFIGUREPATH_H

#include <QPen>
#include <QBrush>
#include <QPainterPath>

#include "finErrorCode.h"

class finFigurePath
{
protected:
    QPen _pen;
    QBrush _brush;
    QPainterPath _path;

public:
    finFigurePath();
    finFigurePath(const finFigurePath &figpath);

    QPen getPen() const;
    QBrush getBrush() const;
    QPainterPath getPath() const;

    finErrorCode setPen(const QPen &pen);
    finErrorCode setBrush(const QBrush &brush);
    finErrorCode setPath(const QPainterPath &path);

    finFigurePath &operator = (const finFigurePath &figpath);
};

#endif // FINFIGUREPATH_H
