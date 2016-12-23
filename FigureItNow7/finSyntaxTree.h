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
    finSyntaxNode *_rootNode;
    QStringList _scriptCodes;
    QList<finSyntaxError *> _errList;

public:
    finSyntaxTree();

    finSyntaxNode *getRootNode() const;
    QString getCodeLine(int line) const;
    int getErrorCount() const;
    finSyntaxError *getSyntaxError(int idx) const;

    finErrorCode setRootNode(finSyntaxNode *rtnode);
    finErrorCode setScriptCode(const QString &script);
    finErrorCode appendSyntaxError(finSyntaxError *synerr);

    QString dumpSyntaxError(int idx) const;
};

#endif // FINSYNTAXTREE_H
