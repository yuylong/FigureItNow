#include "finfigureobject.h"

unsigned int finFigureObject::_NextObjectId = 0;

finFigureObject::finFigureObject()
    : _referCount(1)
{
    this->_objectId = _NextObjectId++;
}

int finFigureObject::retainObject()
{
    int retVal;
    retVal = this->_referCount.fetchAndAddRelaxed(1);
    return retVal + 1;
}

int finFigureObject::releaseObject()
{
    int retVal;
    retVal = this->_referCount.fetchAndSubRelaxed(1);
    if ( retVal == 0 )
        delete (this);
    return retVal - 1;
}

