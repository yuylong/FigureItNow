/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 2nd, 2017
 * Copyright(c) 2017-2026 Yulong Yu. All rights reserved.
 */

#ifndef FINFIGURECONTAINER_H
#define FINFIGURECONTAINER_H

#include <QList>

#include "finErrorCode.h"
#include "finFigureConfig.h"
#include "finGraphConfig.h"
#include "finFigureObject.h"


class finFigureContainer
{
protected:
    finFigureConfig _curFigCfg;
    finGraphConfig _graphCfg;
    QList<finFigureObject *> _figList;

public:
    finFigureContainer();
    ~finFigureContainer();

    const finFigureConfig *getFigureConfig() const;
    finFigureConfig *getFigureConfig();
    const finGraphConfig *getGraphConfig() const;
    finGraphConfig *getGraphConfig();
    void setFigureConfigForObject(finFigureObject *figobj) const;

    int getFigureObjectCount() const;
    finFigureObject *getFigureObjectAt(int idx);

    void appendFigureObject(finFigureObject *figobj);

    void clearFigureObjects();

    void dump() const;
};

#endif // FINFIGURECONTAINER_H
