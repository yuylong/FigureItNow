/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
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
    QChar getScriptCharAt(unsigned long pos) const;
    QChar getScriptChar() const;
    QString getScriptSubAt(unsigned long pos, unsigned long len) const;
    QString getScriptSub(unsigned long len) const;

    finErrorCode moveReadPos();
    finErrorCode moveReadPosWith(unsigned long detpos);
    finErrorCode moveToNextNonblank();

    enum finLexReaderOrder {
        FIN_LR_ORD_NUMBER_FIRST,
        FIN_LR_ORD_OPERATOR_FIRST
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

    bool isVariableStartChar(const QChar &ch);
    bool isVariableChar(const QChar &ch);
};

#endif // FINLEXREADER_H
