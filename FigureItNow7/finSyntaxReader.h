/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */
/*! \file finSyntaxReader.h
 *  \brief The method converting from Lex Nodes to a Syntax Tree.
 *
 * This file gives a machine class that transforming from the lex node to a
 * syntax tree. The syntax tree whose type is finSyntaxNode is a final format
 * from a script code used by FIN-7 for executing the figure drawing task.
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
public:
    enum State {
        ST_DUMMY,
        ST_READY,
        ST_READING,
        ST_DONE,
    };

protected:
    finLexReader _lexReader;

    State _state;
    QList<finSyntaxNode *> _syntaxStack;
    QList<finSyntaxError> _errList;

public:
    finSyntaxReader();

    const finLexReader *getLexReader() const;
    finLexReader *getLexReader();
    QString getScriptCode() const;

    finErrorCode setScriptCode(const QString &scriptcode);

    bool isReading() const;
    State getState() const;

    finErrorCode startRead();
    finErrorCode stopRead();

    finErrorCode readNextToken();
    finSyntaxTree *getSyntaxTree();

private:
    finErrorCode disposeAllRead();

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
