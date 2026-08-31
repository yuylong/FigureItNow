/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finFigurePoint3D.h
 *  \brief Declaration of a lightweight three-dimensional point value.
 *
 *  finFigurePoint3D stores mathematical x, y, and z coordinates and provides component-wise vector
 *  arithmetic used by three-dimensional figure primitives and graph transformations.
 */

#ifndef FINFIGUREPOINT3D_H
#define FINFIGUREPOINT3D_H

#include "finErrorCode.h"


/*! \class finFigurePoint3D
 *  \brief A three-dimensional point or vector in mathematical coordinates.
 *
 *  This value type supports copying, coordinate access, component-wise addition and subtraction,
 *  and multiplication or division by a scalar.
 */
class finFigurePoint3D
{
protected:
    double _x;  //!< X coordinate.
    double _y;  //!< Y coordinate.
    double _z;  //!< Z coordinate.

public:
    /*! \brief Constructs the origin point (0, 0, 0). */
    finFigurePoint3D();
    /*! \brief Constructs a point by copying \a pt. */
    finFigurePoint3D(const finFigurePoint3D &pt);
    /*! \brief Constructs a point from the given x, y, and z coordinates. */
    finFigurePoint3D(double x, double y, double z);

    /*! \name Coordinate Access
     *  \brief Read and modify the individual coordinate components.
     */
    ///@{
    /*! \brief Returns the x coordinate. */
    double getX() const;
    /*! \brief Returns the y coordinate. */
    double getY() const;
    /*! \brief Returns the z coordinate. */
    double getZ() const;

    /*! \brief Sets the x coordinate. */
    void setX(double x);
    /*! \brief Sets the y coordinate. */
    void setY(double y);
    /*! \brief Sets the z coordinate. */
    void setZ(double z);
    ///@}

    /*! \name Arithmetic Operators
     *  \brief Create new three-dimensional values through component-wise or scalar arithmetic.
     */
    ///@{
    /*! \brief Returns the component-wise sum of this point and \a pt. */
    finFigurePoint3D operator + (const finFigurePoint3D &pt);
    /*! \brief Returns the component-wise difference between this point and \a pt. */
    finFigurePoint3D operator - (const finFigurePoint3D &pt);
    /*! \brief Returns this point with every component multiplied by \a scale. */
    finFigurePoint3D operator * (double scale);
    /*! \brief Returns this point with every component divided by \a scale. */
    finFigurePoint3D operator / (double scale);
    /*! \brief Replaces this point's coordinates with those of \a pt. */
    finFigurePoint3D &operator = (const finFigurePoint3D &pt);
    ///@}
};

#endif // FINFIGUREPOINT3D_H
