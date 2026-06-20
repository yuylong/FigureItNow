/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#ifndef FINSYNTAXOPTIMZER_H
#define FINSYNTAXOPTIMZER_H

#include "finErrorCode.h"
#include "finLexNode.h"
#include "finSyntaxTree.h"

#include <QString>
#include <QStringList>


class finSyntaxOptimzer
{
protected:
    finSyntaxTree *_synTree;
    QStringList _optOptions;

public:
    finSyntaxOptimzer();

    finSyntaxTree *getSyntaxTree() const;
    void setSyntaxTree(finSyntaxTree *syntree);
    QStringList getOption() const;
    void setOption(const QStringList &options);

    void optimize();
};

#endif // FINSYNTAXOPTIMZER_H
