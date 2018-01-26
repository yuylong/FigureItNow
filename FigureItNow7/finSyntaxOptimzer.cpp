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
    this->_synTree = NULL;
}

finSyntaxTree *finSyntaxOptimzer::getSyntaxTree()
{
    return this->_synTree;
}

finErrorCode finSyntaxOptimzer::setSyntaxTree(finSyntaxTree *syntree)
{
    this->_synTree = syntree;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxOptimzer::optimize()
{
    if ( this->_synTree == NULL )
        return finErrorKits::EC_NULL_POINTER;
    if ( this->_synTree->getErrorCount() > 0 )
        return finErrorKits::EC_STATE_ERROR;

    finSyntaxNode *rootnode = this->_synTree->getRootNode();
    finErrorCode errcode;

    errcode = optimizeSyntaxNode(rootnode);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxOptimzer::optimizeSyntaxNode(finSyntaxNode *synnode)
{
    if ( synnode == NULL )
        return finErrorKits::EC_NULL_POINTER;

    return finErrorKits::EC_NON_IMPLEMENT;
}

