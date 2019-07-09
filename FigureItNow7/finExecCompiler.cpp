/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
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
    finErrorCode errcode;
    finSyntaxTree *rettree = nullptr;

    if ( this->_scriptCode.length() == 0 )
        return this->buildErrorTree(QString());

    if ( this->_synReader.isReading() )
        this->_synReader.stopRead();

    errcode = this->_synReader.setScriptCode(this->_scriptCode);
    if ( finErrorKits::isErrorResult(errcode) )
        return this->buildErrorTree(QString("Cannot setup script code."));

    errcode = this->_synReader.startRead();
    if ( finErrorKits::isErrorResult(errcode) )
        return this->buildErrorTree(QString("Cannot start syntax reader."));

    while ( errcode != finErrorKits::EC_REACH_BOTTOM ) {
        errcode = this->_synReader.readNextToken();
        if ( finErrorKits::isErrorResult(errcode) )
            break;
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
