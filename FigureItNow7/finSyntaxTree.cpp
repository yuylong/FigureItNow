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

finSyntaxError *finSyntaxTree::getSyntaxError(int idx) const
{
    if ( idx < 0 || idx >= this->_errList.count() )
        return NULL;

    return this->_errList.at(idx);
}

finErrorCode finSyntaxTree::setRootNode(const finSyntaxNode *rtnode)
{
    if ( rtnode == NULL)
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    if ( rtnode->getType() != finSyntaxNode::FIN_SN_TYPE_PROGRAM )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    rtnode->dump();

    return this->_rootNode.copyNode(rtnode);
}

finErrorCode finSyntaxTree::appendSyntaxNode(const finSyntaxNode *synnode)
{
    if ( synnode == NULL)
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode finSyntaxTree::appendSyntaxNodeList(const QList<finSyntaxNode *> *list)
{
    if ( list == NULL)
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode finSyntaxTree::clearSyntaxNodes()
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode finSyntaxTree::setScriptCode(const QString &script)
{
    this->_scriptCodes = script.split(QChar::LineSeparator, QString::KeepEmptyParts);
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode finSyntaxTree::appendSyntaxError(finSyntaxError *synerr)
{
    if ( synerr == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_errList.append(synerr);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finSyntaxTree::disposeAll()
{
    this->_rootNode.disposeAll();
    this->_rootNode.setType(finSyntaxNode::FIN_SN_TYPE_PROGRAM);
    this->_scriptCodes.clear();
    this->_errList.clear();
}

QString finSyntaxTree::dumpSyntaxError(int idx) const
{
    QString retstr;
    QTextStream retstm(&retstr);

    finSyntaxError *synerr = this->getSyntaxError(idx);
    if ( synerr == NULL ) {
        retstm << idx << ": No such error!" << flush;
        return retstr;
    }

    retstm << idx << ": " << synerr->getErrorString() << '\n';

    int row = synerr->getRow();
    int col = synerr->getColumn();
    if ( row < this->_scriptCodes.count() ) {
        retstm << "Code: " << this->_scriptCodes.at(synerr->getRow()) << endl;
        retstm << "      ";
        for ( int i = 0; i < col; i++ )
            retstm << " ";
        retstm << "^" << flush;
    }
    return retstr;
}
