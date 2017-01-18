#ifndef FINFIGUREARROW_H
#define FINFIGUREARROW_H

#include <QString>
#include <QPainterPath>

#include "finErrorCode.h"

class finFigureConfig;
class finGraphConfig;

class finFigureArrow
{
public:
    enum Type {
        FIN_FA_TYPE_NONE,
        FIN_FA_TYPE_TRIANGLE,
    };

    static QString getTypeName(finFigureArrow::Type type);
    static finFigureArrow::Type parseTypeString(const QString &str);

protected:
    finFigureArrow::Type _type;
    double _rad;
    double _length;

public:
    finFigureArrow();
    finFigureArrow(const finFigureArrow &arrow);

    finFigureArrow::Type getType() const;
    double getRadian() const;
    double getLength() const;

    finErrorCode setType(finFigureArrow::Type type);
    finErrorCode setRadian(double rad);
    finErrorCode setLength(double length);

    finFigureArrow &operator = (const finFigureArrow &arrow);
};

typedef finFigureArrow::Type finFigureArrowType;

#endif // FINFIGUREARROW_H
