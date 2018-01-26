/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2018 Yulong Yu. All rights reserved.
 */

#include "finSyntaxOptimzer.h"

#include <QMap>
#include <QString>


typedef finErrorCode (*finSynOptFunc)(finSyntaxNode *synnode);

static finErrorCode _testOptFunc(finSyntaxNode *synnode);

static struct {
    QString _optName;
    finSynOptFunc _func;
} _optFuncList[] = {
    { QString("test"), _testOptFunc },

    { QString(),       NULL         },
};

static QMap<QString, finSynOptFunc> _optFuncMap = QMap<QString, finSynOptFunc>();
static bool _isOptFuncMapInstalled = false;

static finErrorCode _installOptFuncMap()
{
    if ( _isOptFuncMapInstalled )
        return finErrorKits::EC_DUPLICATE_OP;

    for ( int i = 0; _optFuncList[i]._func != NULL; i++ ) {
        _optFuncMap.insert(_optFuncList[i]._optName, _optFuncList[i]._func);
    }
    _isOptFuncMapInstalled = true;
    return finErrorKits::EC_SUCCESS;
}

static QList<finSynOptFunc> _getOptFuncFromOptions(const QStringList &options)
{
    QList<finSynOptFunc> retlist;

    foreach ( QString optname, options ) {
        finSynOptFunc optfunc = _optFuncMap.value(optname, NULL);
        if ( optfunc == NULL )
            continue;

        retlist.append(optfunc);
    }
    return retlist;
}

finSyntaxOptimzer::finSyntaxOptimzer()
    : _optOptions()
{
    this->_synTree = NULL;

    // Install the optimzer function list on the first instance constructed.
    _installOptFuncMap();
}

finSyntaxTree *finSyntaxOptimzer::getSyntaxTree() const
{
    return this->_synTree;
}

finErrorCode finSyntaxOptimzer::setSyntaxTree(finSyntaxTree *syntree)
{
    this->_synTree = syntree;
    return finErrorKits::EC_SUCCESS;
}

QStringList finSyntaxOptimzer::getOption() const
{
    return this->_optOptions;
}

finErrorCode finSyntaxOptimzer::setOption(const QStringList &options)
{
    this->_optOptions = options;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxOptimzer::optimize()
{
    if ( this->_synTree == NULL )
        return finErrorKits::EC_NULL_POINTER;
    if ( this->_synTree->getErrorCount() > 0 )
        return finErrorKits::EC_STATE_ERROR;
    if ( this->_optOptions.isEmpty() )
        return finErrorKits::EC_NORMAL_WARN;

    finSyntaxNode *rootnode = this->_synTree->getRootNode();
    QList<finSynOptFunc> optfunclist = _getOptFuncFromOptions(this->_optOptions);
    finErrorCode errcode = finErrorKits::EC_SUCCESS;
    unsigned funccnt = 0, failedcnt = 0;

    foreach ( finSynOptFunc func, optfunclist ) {
        if ( func == NULL )
            continue;

        funccnt++;
        errcode = func(rootnode);
        if ( finErrorKits::isErrorResult(errcode) )
            failedcnt++;
    }

    if ( failedcnt == 0 )
        return finErrorKits::EC_SUCCESS;
    else if ( failedcnt == funccnt )
        return errcode;
    else
        return finErrorKits::EC_NORMAL_WARN;
}

static finErrorCode _testOptFunc(finSyntaxNode *synnode)
{
    if ( synnode == NULL )
        return finErrorKits::EC_NULL_POINTER;

    return finErrorKits::EC_SUCCESS;
}
