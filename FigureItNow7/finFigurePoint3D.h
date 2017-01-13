#ifndef FINFIGUREPOINT3D_H
#define FINFIGUREPOINT3D_H

#include "finErrorCode.h"

class finFigurePoint3D
{
protected:
    double _x, _y, _z;

public:
    finFigurePoint3D();
    finFigurePoint3D(const finFigurePoint3D &pt);
    finFigurePoint3D(double x, double y, double z);

    double getX() const;
    double getY() const;
    double getZ() const;

    finErrorCode setX(double x);
    finErrorCode setY(double y);
    finErrorCode setZ(double z);

    finFigurePoint3D operator + (const finFigurePoint3D &pt);
    finFigurePoint3D operator - (const finFigurePoint3D &pt);
    finFigurePoint3D operator * (double scale);
    finFigurePoint3D operator / (double scale);
    finFigurePoint3D &operator = (const finFigurePoint3D &pt);
};

#endif // FINFIGUREPOINT3D_H
