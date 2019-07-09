/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2019 Yulong Yu. All rights reserved.
 */

#ifndef FINSYNTAXERRORLIST_H
#define FINSYNTAXERRORLIST_H

#include <QList>
#include <QTextStream>

#include "finSyntaxError.h"


class finSyntaxErrorList
{
protected:
    QList<finSyntaxError> _list;

public:
    finSyntaxErrorList();
};

#endif // FINSYNTAXERRORLIST_H
