#include "finFigureArrow.h"

#include <qmath.h>

finFigureArrow::finFigureArrow()
{
    this->_type = finFigureArrow::FIN_FA_TYPE_NONE;
    this->_rad = M_PI / 6.0;
    this->_length = 6.0;
}

finFigureArrow::finFigureArrow(const finFigureArrow &arrow)
{
    this->_type = arrow._type;
    this->_rad = arrow._rad;
    this->_length = arrow._length;
}

finFigureArrowType finFigureArrow::getType() const
{
    return this->_type;
}

double finFigureArrow::getRadian() const
{
    return this->_rad;
}

double finFigureArrow::getLength() const
{
    return this->_length;
}

finErrorCode finFigureArrow::setType(finFigureArrowType type)
{
    this->_type = type;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureArrow::setRadian(double rad)
{
    rad = rad - floor(rad / (2 * M_PI)) * 2 * M_PI;
    this->_rad = rad;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureArrow::setLength(double length)
{
    if ( length < 0.0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_length = length;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finFigureArrow &finFigureArrow::operator = (const finFigureArrow &arrow)
{
    this->_type = arrow._type;
    this->_rad = arrow._rad;
    this->_length = arrow._length;
    return *this;
}

