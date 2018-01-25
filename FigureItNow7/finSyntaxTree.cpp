/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finSyntaxTree.h"

#include <QTextStream>


finSyntaxTree::finSyntaxTree()
    : _rootNode(), _scriptCodes(), _errList()
{
    this->_rootNode.setType(finSyntaxNode::FIN_SN_TYPE_PROGRAM);
}

finSyntaxTree::~finSyntaxTree()
{
    this->disposeAll();
}

const finSyntaxNode *finSyntaxTree::getRootNode() const
{
    return &this->_rootNode;
}

finSyntaxNode *finSyntaxTree::getRootNode()
{
    return &this->_rootNode;
}

QString finSyntaxTree::getScriptCode() const
{
    QString retstr;
    QTextStream retstm(&retstr);

    for ( int i = 0; i < this->_scriptCodes.count(); i++ ) {
        retstm << this->_scriptCodes.at(i) << endl;
    }
    retstm << flush;
    return retstr;
}

QString finSyntaxTree::getCodeLine(int line) const
{
    if ( line < 0 || line >= this->_scriptCodes.count() )
        return QString();

    return this->_scriptCodes.at(line);
}

int finSyntaxTree::getErrorCount() const
{
    return this->_errList.count();
}

finSyntaxError finSyntaxTree::getSyntaxError(int idx) const
{
    if ( idx < 0 || idx >= this->_errList.count() )
        return finSyntaxError();

    return this->_errList.at(idx);
}

finErrorCode finSyntaxTree::setRootNode(const finSyntaxNode *rtnode)
{
    if ( rtnode == NULL)
        return finErrorKits::EC_NULL_POINTER;

    if ( rtnode->getType() != finSyntaxNode::FIN_SN_TYPE_PROGRAM )
        return finErrorKits::EC_INVALID_PARAM;

    rtnode->dump();

    return this->_rootNode.copyNode(rtnode);
}

finErrorCode finSyntaxTree::appendSyntaxNode(const finSyntaxNode *synnode)
{
    if ( synnode == NULL)
        return finErrorKits::EC_NULL_POINTER;

    //if ( !finSyntaxNode::isStatementLevelType(synnode->getType()) )
    //    return finErrorKits::EC_INVALID_PARAM;

    finSyntaxNode *mynode = new finSyntaxNode();
    if ( mynode == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    finErrorCode errcode = mynode->copyNode(synnode);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete mynode;
        return errcode;
    }

    errcode = this->_rootNode.appendSubSyntaxNode(mynode);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete mynode;
        return errcode;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxTree::prependSyntaxNode(const finSyntaxNode *synnode)
{
    if ( synnode == NULL)
        return finErrorKits::EC_NULL_POINTER;

    //if ( !finSyntaxNode::isStatementLevelType(synnode->getType()) )
    //    return finErrorKits::EC_INVALID_PARAM;

    finSyntaxNode *mynode = new finSyntaxNode();
    if ( mynode == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    finErrorCode errcode = mynode->copyNode(synnode);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete mynode;
        return errcode;
    }

    errcode = this->_rootNode.prependSubSyntaxNode(mynode);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete mynode;
        return errcode;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxTree::appendSyntaxNodeList(const QList<finSyntaxNode *> *list)
{
    if ( list == NULL )
        return finErrorKits::EC_NULL_POINTER;

    for ( int i = 0; i < list->count(); i++ ) {
        const finSyntaxNode *synnode = list->at(i);

        finErrorCode errcode = this->appendSyntaxNode(synnode);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxTree::appendSyntaxNodeStack(const QList<finSyntaxNode *> *list)
{
    if ( list == NULL )
        return finErrorKits::EC_NULL_POINTER;

    for ( int i = list->count() - 1; i >= 0; i-- ) {
        const finSyntaxNode *synnode = list->at(i);

        finErrorCode errcode = this->appendSyntaxNode(synnode);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxTree::clearSyntaxNodes()
{
    return finErrorKits::EC_NON_IMPLEMENT;
}

finErrorCode finSyntaxTree::setScriptCode(const QString &script)
{
    this->_scriptCodes = script.split(QChar::LineSeparator, QString::KeepEmptyParts);
    return finErrorKits::EC_NON_IMPLEMENT;
}

finErrorCode finSyntaxTree::appendSyntaxError(const finSyntaxError &synerr)
{
    this->_errList.append(synerr);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxTree::appendSyntaxErrorList(const QList<finSyntaxError> *list)
{
    this->_errList.append(*list);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxTree::clearSyntaxErrorList()
{
    this->_errList.clear();
    return finErrorKits::EC_SUCCESS;
}

void finSyntaxTree::disposeAll()
{
    this->_rootNode.disposeAll();
    this->_rootNode.setType(finSyntaxNode::FIN_SN_TYPE_PROGRAM);
    this->_scriptCodes.clear();
    this->_errList.clear();
}

QString finSyntaxTree::dumpSyntaxError(const finSyntaxError &synerr, int idx) const
{
    QString retstr;
    QTextStream retstm(&retstr);

    if ( idx > 0 )
        retstm << idx;
    else
        retstm << "err";
    retstm << ": " << synerr.getErrorString() << endl;

    int row = synerr.getRow();
    int col = synerr.getColumn();
    if ( row < this->_scriptCodes.count() ) {
        retstm << "Code: " << this->_scriptCodes.at(synerr.getRow()) << endl;
        retstm << "      ";
        for ( int i = 0; i < col; i++ )
            retstm << " ";
        retstm << "^" << flush;
    }
    return retstr;
}

QString finSyntaxTree::dumpSyntaxError(int idx) const
{
    if ( idx < 0 || idx >= this->_errList.count() ) {
        QString retstr;
        QTextStream retstm(&retstr);

        retstm << idx << ": No such error!" << flush;
        return retstr;
    }

    return this->dumpSyntaxError(this->_errList.at(idx));
}
