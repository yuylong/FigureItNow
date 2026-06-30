/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finExecCompiler.cpp
 *  \brief Implementations of the execution-layer compiler wrapper.
 *
 *  Provides the small adapter that stores script text, drives finSyntaxReader through a full parse,
 *  and converts parse failures into heap-allocated finSyntaxTree instances that carry syntax errors.
 */

#include "finExecCompiler.h"

finExecCompiler::finExecCompiler()
    : _scriptCode(), _synReader()
{
    return;
}

finExecCompiler::~finExecCompiler()
{
    if ( this->_synReader.isReading() )
        this->_synReader.stopRead();
}

QString finExecCompiler::getScriptCode() const
{
    return this->_scriptCode;
}

finErrorCode finExecCompiler::setScriptCode(const QString &script)
{
    this->_scriptCode = script;
    return finErrorKits::EC_SUCCESS;
}

finSyntaxTree *finExecCompiler::compile()
{
    finSyntaxTree *rettree = nullptr;

    if ( this->_scriptCode.length() == 0 )
        return this->buildErrorTree(QString());

    if ( this->_synReader.isReading() )
        this->_synReader.stopRead();

    try {
        this->_synReader.setScriptCode(this->_scriptCode);
        this->_synReader.startRead();

        while ( this->_synReader.readNextToken() ) { }
    } catch (finException &e) {
        return this->buildErrorTree(e.getErrorDescription());
    }

    rettree = this->_synReader.getSyntaxTree();
    if ( rettree == nullptr )
        rettree = this->buildErrorTree(QString("Cannot build syntax tree."));

    this->_synReader.stopRead();
    return rettree;
}

finSyntaxTree *finExecCompiler::buildErrorTree(const QString &errstr) const
{
    finSyntaxTree *rettree = new finSyntaxTree();
    if ( rettree == nullptr )
        return nullptr;

    if ( errstr.length() <= 0 )
        return rettree;

    finSyntaxError synerr;
    synerr.setRow(0);
    synerr.setColumn(0);
    synerr.setErrorString(errstr);

    rettree->appendSyntaxError(synerr);
    return rettree;
}
