/*! \file finSyntaxReader.h
 *  \brief The method converting from Lex Nodes to a Syntax Tree.
 *
 * This file gives a machine class that transforming from the lex node to a
 * syntax tree. The syntax tree whose type is finSyntaxNode is a final format
 * from a script code used by FIN-7 for executing the figure drawing task.
 *
 * Yulong Yu, May 31st, 2016
 * Yusoft (c) All rights reserved, 2016.
 *
 * This file is protected by International Copyright Law. You have no right to
 * distribute, copy, and modify it without the authority by the author, the
 * copyright holders, and Yusoft. Any citations need the hard-copy permit by
 * the author.
 */

#ifndef FINSYNTAXREADER_H
#define FINSYNTAXREADER_H

#include <QList>
#include <QString>

#include "finErrorCode.h"
#include "finLexReader.h"
#include "finSyntaxNode.h"
#include "finSyntaxError.h"
#include "finSyntaxTree.h"


class finSyntaxReader
{
protected:
    QList<finSyntaxNode *> _syntaxStack;
    QList<finSyntaxError *> _errList;
    finSyntaxTree *_syntaxTree;
    bool _isReading;

    finLexReader *_lexReader;
    bool _isExternLexReader;

public:
    finSyntaxReader();

    finLexReader *getLexReader() const;
    QString getScriptCode() const;

    finErrorCode setLexReader(finLexReader *newlexreader);
    finErrorCode setScriptCode(const QString &scriptcode);

    QList<finSyntaxNode *> *getSyntaxStack() { return &this->_syntaxStack; }

    finErrorCode startRead();
    finErrorCode stopRead();

    finErrorCode readNextToken();

    finSyntaxTree *getSyntaxTree();
    finErrorCode disposeSyntaxTree();

private:
    finErrorCode processTypedNextToken(finLexNode *lexnode, finLexNodeType lextype);

    finErrorCode processInstanceToken(finLexNode *lexnode);
    finErrorCode processVirtualToken(finLexNode *lexnode);
    finErrorCode processOperatorToken(finLexNode *lexnode);

    static int getOperatorBfParamCnt(finLexNode *lexnode);
    static int getOperatorAfParamCnt(finLexNode *lexnode);
    static finErrorCode transformOpToNonBfParamOp(finLexNode *lexnode);
    static int getOperatorPriority(finLexNode *lexnode);
    static int getOperatorMeshDir(finLexNode *lexnode);
    static bool isArithOperator(finLexNode *lexnode);
    static int compareOperators(finLexNode *lexnode1, finLexNode *lexnode2);
    finErrorCode processArithOperator(finLexNode *lexnode);

    static bool isLeftBracket(finLexNode *lexnode);
    static bool isRightBracket(finLexNode *lexnode);
    static bool isBracket(finLexNode *lexnode);
    static bool isCorrespnBracket(finLexNode *lexnode1, finLexNode *lexnode2);
    finErrorCode processLeftBracket(finLexNode *lexnode);
    finErrorCode processRightBracket(finLexNode *lexnode);
    finErrorCode processBracket(finLexNode *lexnode);

    static bool isSplitter(finLexNode *lexnode);
    finErrorCode processSplitter(finLexNode *lexnode);

    finErrorCode pushSingleLexNode(finLexNode *lexnode, finSyntaxNodeType tktype);
    finErrorCode meshArithExpress();
    finErrorCode meshAllArithExpress();
    static finSyntaxNode *createDummyExpress();
    finErrorCode meshAllCommas();
    finErrorCode meshStatementWithKeywords();
    finErrorCode recogFlowerBracketType();
    finErrorCode meshFlowerBracket();
    finErrorCode meshRoundBracket();
    finErrorCode meshSquareBracket();
};

#endif // FINSYNTAXREADER_H
