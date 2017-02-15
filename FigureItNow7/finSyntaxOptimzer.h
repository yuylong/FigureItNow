/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#ifndef FINSYNTAXOPTIMZER_H
#define FINSYNTAXOPTIMZER_H

#include "finErrorCode.h"
#include "finLexNode.h"
#include "finSyntaxTree.h"


class finSyntaxOptimzer
{


public:
    finSyntaxOptimzer();

    finErrorCode checkAndOptimize(finSyntaxTree *syntree);

private:
    finErrorCode optimizeSyntaxNode(finSyntaxNode *synnode);
};

#endif // FINSYNTAXOPTIMZER_H
