#ifndef FINEXECVARIABLE_H
#define FINEXECVARIABLE_H


#include <QString>
#include <QList>

#include "finErrorCode.h"

class finExecVariable
{
public:
    enum finExecVariableType {
        FIN_VR_TYPE_DUMMY = 0,
        FIN_VR_TYPE_NULL,
        FIN_VR_TYPE_NUMERIC,
        FIN_VR_TYPE_STRING,
        FIN_VR_TYPE_ARRAY,
        FIN_VR_TYPE_MAX
    };

protected:
    finExecVariableType _type;
    QString _varName;
    bool _writeProtect;
    bool _leftValue;

    double _numVal;
    QString _strVal;
    QList<finExecVariable *> _itemList;

public:
    finExecVariable();
    finExecVariable(const QString &name);

    QString getName() const;
    bool isSameName(const QString &name) const;
    finExecVariableType getType() const;
    bool isWriteProtected() const;
    bool isLeftValue() const;

    double getNumericValue() const;
    QString getStringValue() const;

    finExecVariable *getVariableItemAt(int idx) const;
    finExecVariable *getVariableItemAt(int idx);
    int getArrayLength() const;

    bool isNumericMatrix(int *rowcnt = NULL, int *colcnt = NULL) const;
    bool isNumericArray(int *cnt = NULL) const;
    bool isStringArray(int *cnt = NULL) const;

    finErrorCode setName(const QString &name);
    finErrorCode setType(finExecVariableType type);
    finErrorCode setWriteProtected();
    finErrorCode clearWriteProtected();
    finErrorCode setLeftValue();
    finErrorCode clearLeftValue();

    finErrorCode setNumericValue(double val);
    finErrorCode setStringValue(const QString &strval);

    finErrorCode copyVariable(const finExecVariable *srcvar);
    finErrorCode appendList(const QList<finExecVariable *> &vallist);

    finErrorCode dispose();

private:
    finErrorCode copyArrayVariable(const finExecVariable *srcvar);
};

typedef enum finExecVariable::finExecVariableType finExecVariableType;

#endif // FINEXECVARIABLE_H
