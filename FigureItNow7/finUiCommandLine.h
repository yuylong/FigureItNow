/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#ifndef FINUICOMMANDLINE_H
#define FINUICOMMANDLINE_H

#include <QObject>
#include <QString>
#include <QStringList>

#include "finErrorCode.h"
#include "finFigureContainer.h"


class finUiCommandLine : public QObject
{
    Q_OBJECT

protected:
    QStringList _inFileList;
    QString _outType;

public:
    finUiCommandLine();
    finUiCommandLine(int argc, char *argv[]);
    finUiCommandLine(const QStringList &arglist);

    finErrorCode reset();

    finErrorCode parseArgument(int argc, char *argv[]);
    finErrorCode parseArgument(const QStringList &arglist);

    finErrorCode work();

private:
    QStringList parseStringList(int argc, char *argv[]);
    QString parseArgumentCommand(const QString &argstr);

    finErrorCode compileAndRunScript(const QString &filename, finFigureContainer *outfig);
    int figureToPDF();
    int figureToSVG();
    int figureToImage();
};

#endif // FINUICOMMANDLINE_H
