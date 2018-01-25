/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finSyntaxOptimzer.h"


finSyntaxOptimzer::finSyntaxOptimzer()
{
    return;
}

finErrorCode finSyntaxOptimzer::checkAndOptimize(finSyntaxTree *syntree)
{
    if ( syntree == NULL )
        return finErrorKits::EC_NULL_POINTER;

    return finErrorKits::EC_NON_IMPLEMENT;
}

