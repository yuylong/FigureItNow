/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#include "finSyntaxOptimzer.h"

#include <QMap>
#include <QString>


typedef void (*finSynOptFunc)(finSyntaxNode *synnode);

static void _testOptFunc(finSyntaxNode *synnode);

static struct {
    QString _optName;
    finSynOptFunc _func;
} _optFuncList[] = {
    { QString("test"), _testOptFunc },

    { QString(),       nullptr      },
};

static QMap<QString, finSynOptFunc> _optFuncMap = QMap<QString, finSynOptFunc>();
static bool _isOptFuncMapInstalled = false;

static void _installOptFuncMap()
{
    if ( _isOptFuncMapInstalled )
        return;

    for ( int i = 0; _optFuncList[i]._func != nullptr; i++ ) {
        _optFuncMap.insert(_optFuncList[i]._optName, _optFuncList[i]._func);
    }
    _isOptFuncMapInstalled = true;
}

static QList<finSynOptFunc> _getOptFuncFromOptions(const QStringList &options)
{
    QList<finSynOptFunc> retlist;

    foreach ( QString optname, options ) {
        finSynOptFunc optfunc = _optFuncMap.value(optname, nullptr);
        if ( optfunc == nullptr )
            continue;

        retlist.append(optfunc);
    }
    return retlist;
}

finSyntaxOptimzer::finSyntaxOptimzer()
    : _optOptions()
{
    this->_synTree = nullptr;

    // Install the optimzer function list on the first instance constructed.
    _installOptFuncMap();
}

finSyntaxTree *finSyntaxOptimzer::getSyntaxTree() const
{
    return this->_synTree;
}

void finSyntaxOptimzer::setSyntaxTree(finSyntaxTree *syntree)
{
    this->_synTree = syntree;
}

QStringList finSyntaxOptimzer::getOption() const
{
    return this->_optOptions;
}

void finSyntaxOptimzer::setOption(const QStringList &options)
{
    this->_optOptions = options;
}

void finSyntaxOptimzer::optimize()
{
    if ( this->_synTree == nullptr )
        finThrow(finErrorKits::EC_NULL_POINTER, "Cannot optimize a null syntax tree.");
    if ( this->_synTree->getErrorCount() > 0 )
        finThrow(finErrorKits::EC_STATE_ERROR, "Cannot optimize a syntax tree with errors.");
    if ( this->_optOptions.isEmpty() )
        return;

    finSyntaxNode *rootnode = this->_synTree->getRootNode();
    QList<finSynOptFunc> optfunclist = _getOptFuncFromOptions(this->_optOptions);

    foreach ( finSynOptFunc func, optfunclist ) {
        if ( func == nullptr )
            continue;

        func(rootnode);
    }
}

static void _testOptFunc(finSyntaxNode *synnode)
{
    if ( synnode == nullptr )
        return;
}
