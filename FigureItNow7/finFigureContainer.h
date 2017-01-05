/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 2nd, 2017
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20161230    0  Yulong Yu    Create this file.
 * 20170102    1  Yulong Yu    Add graph config into figure container.
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

    const finFigureConfig *getFigureConfig() const;
    finFigureConfig *getFigureConfig();
    const finGraphConfig *getGraphConfig() const;
    finGraphConfig *getGraphConfig();
    finErrorCode setFigureConfigForObject(finFigureObject *figobj) const;

    int getFigureObjectCount() const;
    finFigureObject *getFigureObjectAt(int idx);

    finErrorCode appendFigureObject(finFigureObject *figobj);

    void dump() const;
};

#endif // FINFIGURECONTAINER_H
