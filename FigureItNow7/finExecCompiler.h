#ifndef FINEXECCOMPILER_H
#define FINEXECCOMPILER_H

#include "finErrorCode.h"
#include "finSyntaxReader.h"
#include "finSyntaxTree.h"


class finExecCompiler
{
private:
    QString _scriptCode;
    finSyntaxReader _synReader;

public:
    finExecCompiler();
    ~finExecCompiler();

    QString getScriptCode() const;
    finErrorCode setScriptCode(const QString &script);

    finSyntaxTree *compile();

private:
    finSyntaxTree *buildErrorTree(const QString &errstr) const;
};

#endif // FINEXECCOMPILER_H
