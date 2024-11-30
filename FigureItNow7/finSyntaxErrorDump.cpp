/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2019 Yulong Yu. All rights reserved.
 */

#include "finSyntaxErrorDump.h"

#include <iostream>
#include <QByteArray>


finSyntaxErrorDump::finSyntaxErrorDump()
{
    return;
}

finSyntaxErrorDump::~finSyntaxErrorDump()
{
    return;
}

finSyntaxErrorDumpCout::finSyntaxErrorDumpCout()
{
    return;
}

finSyntaxErrorDumpCout::~finSyntaxErrorDumpCout()
{
    return;
}

void finSyntaxErrorDumpCout::dumpText(const QString &text)
{
    QByteArray ba = text.toUtf8();
    std::cout << ba.data() << std::endl;
}


finSyntaxErrorDumpGUIPlainText::finSyntaxErrorDumpGUIPlainText()
{
    this->_widget = nullptr;
}

finSyntaxErrorDumpGUIPlainText::finSyntaxErrorDumpGUIPlainText(QPlainTextEdit *widget)
{
    this->_widget = widget;
}

finSyntaxErrorDumpGUIPlainText::~finSyntaxErrorDumpGUIPlainText()
{
    return;
}

QPlainTextEdit *finSyntaxErrorDumpGUIPlainText::getWidget() const
{
    return this->_widget;
}

finErrorCode finSyntaxErrorDumpGUIPlainText::setWidget(QPlainTextEdit *widget)
{
    this->_widget = widget;
    return finErrorKits::EC_SUCCESS;
}

void finSyntaxErrorDumpGUIPlainText::dumpText(const QString &text)
{
    if ( this->_widget == nullptr )
        return;

    this->_widget->appendPlainText(text);
}
