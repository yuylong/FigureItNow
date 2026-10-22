/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#include "finSyntaxTree.h"

#include <QtGlobal>
#include <QTextStream>


finSyntaxTree::finSyntaxTree()
    : _rootNode(), _scriptCodes()
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

void finSyntaxTree::setScriptCode(const QString &script)
{
    this->_scriptCodes = script.split(QChar::LineSeparator, Qt::KeepEmptyParts);
}

QString finSyntaxTree::dumpObjInfo() const
{
    QString retstr;
    QTextStream ts(&retstr);
    QString firstscln = "NULL";
    if ( !this->_scriptCodes.empty() )
        firstscln = this->_scriptCodes[0];

    ts << "Root:{" << this->_rootNode.dumpObjInfo() << "}; "
       << "Script:[" << this->_scriptCodes.count() << "]{" << firstscln << "};";

    return retstr;
}

void finSyntaxTree::disposeAll()
{
    this->_rootNode.disposeAll();
    this->_rootNode.setType(finSyntaxNode::TP_PROGRAM);
    this->_scriptCodes.clear();
}
