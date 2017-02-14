#ifndef FINUICOMMANDLINE_H
#define FINUICOMMANDLINE_H

#include <QString>
#include <QStringList>

#include "finErrorCode.h"
#include "finFigureContainer.h"


class finUiCommandLine
{
protected:
    QStringList _inFileList;
    QString _outType;

public:
    finUiCommandLine();
    finUiCommandLine(int argc, const char *argv[]);
    finUiCommandLine(const QStringList &arglist);

    finErrorCode reset();

    finErrorCode parseArgument(int argc, const char *argv[]);
    finErrorCode parseArgument(const QStringList &arglist);

    finErrorCode work();

private:
    QStringList parseStringList(int argc, const char *argv[]);
    QString parseArgumentCommand(const QString &argstr);

    finErrorCode compileAndRunScript(const QString &filename, finFigureContainer *outfig);
    finErrorCode figureToPDF();

};

#endif // FINUICOMMANDLINE_H
