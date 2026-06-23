/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
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


class finSyntaxReader : public finExceptionObject
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

    void setScriptCode(const QString &scriptcode);

    bool isReading() const;
    State getState() const;

    void startRead();
    void stopRead();

    bool readNextToken();
    finSyntaxTree *getSyntaxTree();

    virtual QString dumpObjInfo() const override;

private:
    void disposeAllRead();

    void processTypedNextToken(finLexNode *lexnode, finLexNodeType lextype);

    void processInstanceToken(finLexNode *lexnode);
    void processVirtualToken(finLexNode *lexnode);
    void processOperatorToken(finLexNode *lexnode);

    static int getOperatorBfParamCnt(finLexNode *lexnode);
    static int getOperatorAfParamCnt(finLexNode *lexnode);
    static void transformOpToNonBfParamOp(finLexNode *lexnode);
    static int getOperatorPriority(finLexNode *lexnode);
    static int getOperatorMeshDir(finLexNode *lexnode);
    static bool isArithOperator(finLexNode *lexnode);
    static int compareOperators(finLexNode *lexnode1, finLexNode *lexnode2);
    void processArithOperator(finLexNode *lexnode);

    static bool isLeftBracket(finLexNode *lexnode);
    static bool isRightBracket(finLexNode *lexnode);
    static bool isBracket(finLexNode *lexnode);
    static bool isCorrespnBracket(finLexNode *lexnode1, finLexNode *lexnode2);
    void processLeftBracket(finLexNode *lexnode);
    void processRightBracket(finLexNode *lexnode);
    void processBracket(finLexNode *lexnode);

    static bool isSplitter(finLexNode *lexnode);
    void processSplitter(finLexNode *lexnode);

    void pushSingleLexNode(finLexNode *lexnode, finSyntaxNodeType tktype);
    bool meshArithExpress();
    void meshAllArithExpress();
    static finSyntaxNode *createDummyExpress();
    void meshAllCommas();
    void meshStatementWithKeywords();
    void recogFlowerBracketType();
    void meshFlowerBracket();
    void meshRoundBracket();
    void meshSquareBracket();
};

#endif // FINSYNTAXREADER_H
