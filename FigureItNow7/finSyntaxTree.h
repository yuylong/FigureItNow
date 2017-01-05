#ifndef FINSYNTAXTREE_H
#define FINSYNTAXTREE_H


#include <QList>
#include <QString>
#include <QStringList>

#include "finErrorCode.h"
#include "finSyntaxNode.h"
#include "finSyntaxError.h"

class finSyntaxTree
{
protected:
    finSyntaxNode _rootNode;
    QStringList _scriptCodes;
    QList<finSyntaxError *> _errList;

public:
    finSyntaxTree();
    ~finSyntaxTree();

    const finSyntaxNode *getRootNode() const;
    finSyntaxNode *getRootNode();
    QString getCodeLine(int line) const;
    int getErrorCount() const;
    finSyntaxError *getSyntaxError(int idx) const;

    finErrorCode setRootNode(const finSyntaxNode *rtnode);
    finErrorCode appendSyntaxNode(const finSyntaxNode *synnode);
    finErrorCode appendSyntaxNodeList(const QList<finSyntaxNode *> *list);
    finErrorCode clearSyntaxNodes();

    finErrorCode setScriptCode(const QString &script);
    finErrorCode appendSyntaxError(finSyntaxError *synerr);

    void disposeAll();

    QString dumpSyntaxError(int idx) const;
};

#endif // FINSYNTAXTREE_H
