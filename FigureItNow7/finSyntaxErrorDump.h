/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2019 Yulong Yu. All rights reserved.
 */

#ifndef FINSYNTAXERRORDUMP_H
#define FINSYNTAXERRORDUMP_H

#include <QString>
#include <QPlainTextEdit>

#include "finErrorCode.h"


class finSyntaxErrorDump
{
public:
    finSyntaxErrorDump();
    virtual ~finSyntaxErrorDump();

    virtual void dumpText(const QString &text) = 0;
};


class finSyntaxErrorDumpCout : public finSyntaxErrorDump
{
    finSyntaxErrorDumpCout();
    virtual ~finSyntaxErrorDumpCout();

    virtual void dumpText(const QString &text);
};

class finSyntaxErrorDumpGUIPlainText : public finSyntaxErrorDump
{
private:
    QPlainTextEdit *_widget;

public:
    finSyntaxErrorDumpGUIPlainText();
    finSyntaxErrorDumpGUIPlainText(QPlainTextEdit *widget);
    virtual ~finSyntaxErrorDumpGUIPlainText();

    QPlainTextEdit *getWidget() const;
    finErrorCode setWidget(QPlainTextEdit *widget);

    virtual void dumpText(const QString &text);
};

#endif // FINSYNTAXERRORDUMP_H
