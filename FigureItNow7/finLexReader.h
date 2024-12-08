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


class finLexReader
{
private:
    QString _inputStr;
    unsigned long _posIdx;
    unsigned int _curRow, _curCol;

    finLexNodeType _lastNodeType;

public:
    finLexReader();
    finLexReader(const QString &inputstr);

    QString getString() const;
    unsigned long getCurrentPosition() const;
    finErrorCode setString(const QString &instr);
    finErrorCode resetPosition();

    finErrorCode getNextLexNode(finLexNode *retnode);

private:
    unsigned long scriptLength() const;
    QChar getScriptCharAt(unsigned long pos) const;
    QChar getScriptChar() const;
    QString getScriptSubAt(unsigned long pos, unsigned long len) const;
    QString getScriptSub(unsigned long len) const;

    finErrorCode moveReadPos();
    finErrorCode moveReadPosWith(unsigned long detpos);
    finErrorCode moveToNextNonblank();

    finErrorCode buildLexNode(finLexNode *retnode, finLexNodeType type, unsigned long endpos);

    enum finLexReaderOrder {
        ORD_NUMBER_FIRST,
        ORD_OPERATOR_FIRST
    };

    finLexReaderOrder _nextReadOrder;

    finErrorCode getLexTypeOrder(const finLexNodeType **typeorder, int *typenum);
    finErrorCode getLexTypeOrder(finLexReader::finLexReaderOrder order,
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
