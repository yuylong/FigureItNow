/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#ifndef FINSYNTAXTREE_H
#define FINSYNTAXTREE_H

#include <QList>
#include <QString>
#include <QStringList>

#include "finErrorCode.h"
#include "finSyntaxNode.h"
#include "finSyntaxError.h"


class finSyntaxTree : public finExceptionObject
{
protected:
    finSyntaxNode _rootNode;
    QStringList _scriptCodes;
    QList<finSyntaxError> _errList;

public:
    finSyntaxTree();
    virtual ~finSyntaxTree();

    const finSyntaxNode *getRootNode() const;
    finSyntaxNode *getRootNode();
    QString getScriptCode() const;
    QString getCodeLine(int line) const;
    int getErrorCount() const;
    finSyntaxError getSyntaxError(int idx) const;

    void setRootNode(const finSyntaxNode *rtnode);
    void appendSyntaxNode(const finSyntaxNode *synnode);
    void prependSyntaxNode(const finSyntaxNode *synnode);
    void appendSyntaxNodeList(const QList<finSyntaxNode *> *list);
    void appendSyntaxNodeStack(const QList<finSyntaxNode *> *list);
    void clearSyntaxNodes();

    void setScriptCode(const QString &script);

    void appendSyntaxError(const finSyntaxError &synerr);
    void appendSyntaxErrorList(const QList<finSyntaxError> *list);
    void clearSyntaxErrorList();

    virtual QString dumpObjInfo() const override;

    void disposeAll();

    QString dumpSyntaxError(const finSyntaxError &synerr, int idx = -1) const;
    QString dumpSyntaxError(int idx) const;
};

#endif // FINSYNTAXTREE_H
