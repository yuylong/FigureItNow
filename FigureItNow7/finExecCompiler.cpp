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
    this->_errList = nullptr;
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

void finExecCompiler::setScriptCode(const QString &script)
{
    this->_scriptCode = script;
}

void finExecCompiler::setErrorList(finSyntaxErrorList *errlist)
{
    this->_errList = errlist;
    this->_synReader.setErrorList(errlist);
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

    // Script-level errors are recorded directly into the shared error list before they are thrown,
    // so each one carries its own source position and classification. Only record a fallback entry
    // when nothing was recorded (e.g. an internal error thrown outside the reader's reporting
    // path): adding the thrown message as well would report the same problem twice.
    if ( this->_errList == nullptr || this->_errList->getEntryCount() > 0 )
        return rettree;

    // Nothing was recorded: fall back to a single entry carrying just the message, at a reported
    // position of (0, 0).
    finSyntaxError synerr;
    synerr.setErrorInfo(finSyntaxError::LV_ERROR, finSyntaxError::ST_COMPILE,
                        finSyntaxError::CD_DUMMY, nullptr, errstr);

    this->_errList->appendEntry(synerr);
    return rettree;
}
