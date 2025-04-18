/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2024 Yulong Yu. All rights reserved.
 */

#ifndef FINLEXREADER_H
#define FINLEXREADER_H

#include <QString>

#include "finErrorCode.h"
#include "finLexNode.h"


class finLexReader : finExceptionObject
{
private:
    QString _inputStr;
    unsigned long _posIdx;
    unsigned int _curRow, _curCol;

    finLexNodeType _lastNodeType;

public:
    finLexReader();
    finLexReader(const QString &inputstr);
    ~finLexReader();

    QString getString() const;
    unsigned long getCurrentPosition() const;
    bool isReachBottom() const;
    void setString(const QString &instr);
    void resetPosition();

    finErrorCode getNextLexNode(finLexNode *retnode);

    virtual QString dumpObjInfo() const override;

private:
    static QString dbgScript(const QString str);

    unsigned long scriptLength() const;
    QChar getScriptCharAt(unsigned long pos) const;
    QChar getScriptChar() const;
    QString getScriptSubAt(unsigned long pos, unsigned long len) const;
    QString getScriptSub(unsigned long len) const;

    finErrorCode moveReadPos();
    finErrorCode moveReadPosWith(unsigned long detpos);
    finErrorCode moveToNextNonblank();

    void buildLexNode(finLexNode *retnode, finLexNodeType type, unsigned long endpos);

    enum finLexReaderOrder {
        ORD_NUMBER_FIRST,
        ORD_OPERATOR_FIRST
    };

    finLexReaderOrder _nextReadOrder;

    void getLexTypeOrder(const finLexNodeType **typeorder, int *typenum);
    void getLexTypeOrder(finLexReader::finLexReaderOrder order,
                         const finLexNodeType **typeorder, int *typenum);
    finErrorCode tryGetTypedNode(finLexNode *retnode, finLexNodeType lextype);
    finErrorCode tryGetNote(finLexNode *retnode);
    finErrorCode tryGetVariable(finLexNode *retnode);
    finErrorCode tryRecogKeyword(finLexNode *retnode);
    finErrorCode tryGetNumber(finLexNode *retnode);
    finErrorCode tryGetString(finLexNode *retnode);
    finErrorCode tryGetOperator(finLexNode *retnode);

    static bool isVariableStartChar(const QChar &ch);
    static bool isVariableChar(const QChar &ch);
};

#endif // FINLEXREADER_H
