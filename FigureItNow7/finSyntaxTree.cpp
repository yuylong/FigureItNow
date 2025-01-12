/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2024 Yulong Yu. All rights reserved.
 */

#include "finSyntaxTree.h"

#include <QtGlobal>
#include <QTextStream>


finSyntaxTree::finSyntaxTree()
    : _rootNode(), _scriptCodes(), _errList()
{
    this->_rootNode.setType(finSyntaxNode::TP_PROGRAM);
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
        retstm << this->_scriptCodes.at(i) << Qt::endl;
    }
    return retstr;
}

QString finSyntaxTree::getCodeLine(int line) const
{
    if ( line < 0 || line >= this->_scriptCodes.count() )
        return QString();

    return this->_scriptCodes[line];
}

int finSyntaxTree::getErrorCount() const
{
    return this->_errList.count();
}

finSyntaxError finSyntaxTree::getSyntaxError(int idx) const
{
    if ( idx < 0 || idx >= this->_errList.count() )
        return finSyntaxError();

    return this->_errList[idx];
}

void finSyntaxTree::setRootNode(const finSyntaxNode *rtnode)
{
    Q_ASSERT(rtnode != nullptr);

    if ( rtnode->getType() != finSyntaxNode::TP_PROGRAM )
        finThrowObj(finErrorKits::EC_INVALID_PARAM, "Root Node must be program type.");

    rtnode->dump();

    this->_rootNode.copyNode(rtnode);
}

void finSyntaxTree::appendSyntaxNode(const finSyntaxNode *synnode)
{
    Q_ASSERT(synnode != nullptr);

    finSyntaxNode *mynode = new finSyntaxNode();
    if (mynode == nullptr)
        finThrowObj(finErrorKits::EC_OUT_OF_MEMORY, "Alloc syntax node failed.");

    try {
        mynode->copyNode(synnode);
        this->_rootNode.appendSubSyntaxNode(mynode);
    } catch (finException &e) {
        delete mynode;
        throw e;
    }
}

void finSyntaxTree::prependSyntaxNode(const finSyntaxNode *synnode)
{
    Q_ASSERT(synnode != nullptr);

    finSyntaxNode *mynode = new finSyntaxNode();
    if (mynode == nullptr)
        finThrowObj(finErrorKits::EC_OUT_OF_MEMORY, "Alloc syntax node failed.");

    try {
        mynode->copyNode(synnode);
        this->_rootNode.prependSubSyntaxNode(mynode);
    } catch (finException &e) {
        delete mynode;
        throw e;
    }
}

void finSyntaxTree::appendSyntaxNodeList(const QList<finSyntaxNode *> *list)
{
    Q_ASSERT(list != nullptr);

    for ( int i = 0; i < list->count(); i++ ) {
        const finSyntaxNode *synnode = list->at(i);
        if (synnode == nullptr) {
            // TODO: This condition need?
            finWarning << "Null syntax node in list at " << i;
            continue;
        }

        this->appendSyntaxNode(synnode);
    }
}

void finSyntaxTree::appendSyntaxNodeStack(const QList<finSyntaxNode *> *list)
{
    Q_ASSERT(list != nullptr);

    for ( int i = list->count() - 1; i >= 0; i-- ) {
        const finSyntaxNode *synnode = list->at(i);
        if (synnode == nullptr) {
            // TODO: This condition need?
            finWarning << "Null syntax node in list at " << i;
            continue;
        }

        this->appendSyntaxNode(synnode);
    }
}

void finSyntaxTree::clearSyntaxNodes()
{
    finThrowObj(finErrorKits::EC_NON_IMPLEMENT, "Not implemented.");
}

finErrorCode finSyntaxTree::setScriptCode(const QString &script)
{
    this->_scriptCodes = script.split(QChar::LineSeparator, Qt::KeepEmptyParts);
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

QString finSyntaxTree::dumpObjInfo() const
{
    QString retstr;
    QTextStream ts(&retstr);
    QString firstscln = "NULL";
    if ( !this->_scriptCodes.empty() )
        firstscln = this->_scriptCodes[0];

    ts << "Root:{" << this->_rootNode.dumpObjInfo() << "}; "
       << "Script:[" << this->_scriptCodes.count() << "]{" << firstscln << "}; "
       << "Errors:[" << this->_errList.count() <<"];";

    return retstr;
}

void finSyntaxTree::disposeAll()
{
    this->_rootNode.disposeAll();
    this->_rootNode.setType(finSyntaxNode::TP_PROGRAM);
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
    retstm << ": " << synerr.getErrorString() << Qt::endl;

    int row = synerr.getRow();
    int col = synerr.getColumn();
    if ( row < this->_scriptCodes.count() ) {
        retstm << "Code: " << this->_scriptCodes.at(synerr.getRow()) << Qt::endl;
        retstm << "      ";
        for ( int i = 0; i < col; i++ )
            retstm << " ";
        retstm << "^" << Qt::flush;
    }
    return retstr;
}

QString finSyntaxTree::dumpSyntaxError(int idx) const
{
    if ( idx < 0 || idx >= this->_errList.count() ) {
        QString retstr;
        QTextStream retstm(&retstr);

        retstm << idx << ": No such error!" << Qt::flush;
        return retstr;
    }

    return this->dumpSyntaxError(this->_errList.at(idx));
}
