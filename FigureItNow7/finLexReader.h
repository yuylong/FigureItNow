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


class finLexReader : public finExceptionObject
{
private:
    QString _inputStr;
    unsigned long _posIdx;
    unsigned int _curRow, _curCol;

    finLexNodeType _lastNodeType;

public:
    finLexReader();
    finLexReader(const QString &inputstr);
    virtual ~finLexReader();

    QString getString() const;
    unsigned long getCurrentPosition() const;
    bool isReachBottom() const;
    void setString(const QString &instr);
    void resetPosition();

    bool getNextLexNode(finLexNode *retnode);

    virtual QString dumpObjInfo() const override;

private:
    static QString dbgScript(const QString str);

    unsigned long scriptLength() const;
    QChar getScriptCharAt(unsigned long pos) const;
    QChar getScriptChar() const;
    QString getScriptSubAt(unsigned long pos, unsigned long len) const;
    QString getScriptSub(unsigned long len) const;

    bool moveReadPos();
    bool moveReadPosWith(unsigned long detpos);
    bool moveToNextNonblank();

    void buildLexNode(finLexNode *retnode, finLexNodeType type, unsigned long endpos);

    enum finLexReaderOrder {
        ORD_NUMBER_FIRST,
        ORD_OPERATOR_FIRST
    };

    finLexReaderOrder _nextReadOrder;

    void getLexTypeOrder(const finLexNodeType **typeorder, int *typenum);
    void getLexTypeOrder(finLexReader::finLexReaderOrder order,
                         const finLexNodeType **typeorder, int *typenum);
    bool tryGetTypedNode(finLexNode *retnode, finLexNodeType lextype);
    bool tryGetNote(finLexNode *retnode);
    bool tryGetVariable(finLexNode *retnode);
    bool tryRecogKeyword(finLexNode *retnode);
    bool tryGetNumber(finLexNode *retnode);
    bool tryGetString(finLexNode *retnode);
    bool tryGetOperator(finLexNode *retnode);

    static bool isVariableStartChar(const QChar &ch);
    static bool isVariableChar(const QChar &ch);
};

#endif // FINLEXREADER_H
