#ifndef FINEXECFLOWCONTROL_H
#define FINEXECFLOWCONTROL_H


#include <QString>

#include "finErrorCode.h"

class finExecFlowControl
{
public:
    enum finExecFlowControlType {
        FIN_FC_NEXT,
        FIN_FC_RETURN,
        FIN_FC_CONTINUE,
        FIN_FC_BREAK,
        FIN_FC_GOTO,
        FIN_FC_EXIT,
        FIN_FC_ERROR
    };

protected:
    finExecFlowControlType _type;
    QString _label;

public:
    finExecFlowControl();

    finExecFlowControlType getType() const;
    bool isFlowNext() const;
    bool isFlowGoto() const;
    QString getGotoLabel() const;

    finErrorCode setType(finExecFlowControlType type);
    finErrorCode setLabel(const QString &label);
    finErrorCode setFlowNext();
    finErrorCode setGotoAndLabel(const QString &label);

    finErrorCode directPass();
};

typedef finExecFlowControl::finExecFlowControlType finExecFlowControlType;

#endif // FINEXECFLOWCONTROL_H
