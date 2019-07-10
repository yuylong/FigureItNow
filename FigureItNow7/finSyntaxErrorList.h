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
#include <QString>
#include <QTextStream>

#include "finErrorCode.h"
#include "finLexNode.h"
#include "finSyntaxError.h"


class finSyntaxErrorList
{
protected:
    QList<finSyntaxError> _list;

public:
    finSyntaxErrorList();

    const finSyntaxError &getEntryAt(int index) const;

    finErrorCode appendEntry(finSyntaxError::Level level, finSyntaxError::Stage stage,
                             finLexNode *lexnode, QString info);
    finErrorCode removeEntryAt(int index);
    void clearAllErrorList();
};

#endif // FINSYNTAXERRORLIST_H
