#ifndef FINFIGUREOBJECT_H
#define FINFIGUREOBJECT_H

#include <QAtomicInteger>


class finFigureObject
{
public:
    enum finFigureObjectType {
        FIN_FO_TYPE_DUMMY,
        FIN_FO_TYPE_DOT,
        FIN_FO_TYPE_LINE,
        FIN_FO_TYPE_RECT,
        FIN_FO_TYPE_POLYLINE,
        FIN_FO_TYPE_MAX
    };

private:
    static unsigned int _NextObjectId;

    unsigned int _objectId;
    QAtomicInteger<int> _referCount;

public:
    finFigureObject();

    virtual int retainObject();
    virtual int releaseObject();
};

#endif // FINFIGUREOBJECT_H
