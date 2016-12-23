#ifndef FINEXECFUNCTION_H
#define FINEXECFUNCTION_H

#include <QString>

#include "finErrorCode.h"
#include "finSyntaxNode.h"
#include "finExecVariable.h"

class finExecEnvironment;

class finExecFunction
{
public:
    enum finExecFunctionType {
        FIN_FN_TYPE_DUMMY,
        FIN_FN_TYPE_USER,
        FIN_FN_TYPE_SYSTEM,
        FIN_FIN_TYPE_MAX
    };

protected:
    finExecFunctionType _type;
    QString _funcName;
    QStringList _paramList;

    union {
        finSyntaxNode *_funcNode;
        finErrorCode (*_funcCall)(finExecFunction *self,
                                  finExecVariable *retval,
                                  finExecEnvironment *execenv);
        void *_rawPointer;
    } _u;

public:
    finExecFunction();

    finExecFunctionType getFunctionType() const;
    QString getFunctionName() const;
    int getParameterCount() const;
    QString getParameterName(int idx) const;

    finErrorCode setFunctionType(finExecFunctionType type);
    finErrorCode setFunctionName(const QString &funcname);
    finErrorCode appendParameterName(const QString &paramname);
    finErrorCode clearParameterNames();

    finErrorCode execFunction(finSyntaxNode *curnode, finExecVariable *retval,
                              finExecEnvironment *execenv);

private:
    finErrorCode execUserFunction(finExecVariable *retval,
                                  finExecEnvironment *execenv);
    finErrorCode execSysFunction(finExecVariable *retval,
                                 finExecEnvironment *execenv);

};

typedef finExecFunction::finExecFunctionType finExecFunctionType;

#endif // FINEXECFUNCTION_H
