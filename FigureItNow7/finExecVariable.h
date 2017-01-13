/*! \file finExecVariable.h
 *  \brief The script variable declaration.
 *
 * This file declares the script runtime variable. A variable is not only the explicit user-defined in their scripts,
 * but also the temporary results of the operator calculations and the function return values.
 */
/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, May 26th, 2016
 * Copyright(c) 2015-2016 Yulong Yu. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20151229    0  Yulong Yu    Create this file.
 */

#ifndef FINEXECVARIABLE_H
#define FINEXECVARIABLE_H


#include <QString>
#include <QList>

#include "finErrorCode.h"

class finExecVariable;
class finExecFunction;
class finExecEnvironment;
class finExecMachine;

/*! \class finExecVariable
 *  \brief The script runtime variable.
 *
 * This class is the container of the runtime data. The variables store the explicit user defined variable, constants
 * in their scripts, and also store the return values from operator calculation and the function execution. In FIN-7
 * system, there are three valid type of variables, number, string, and array. A number is a real number, a string is
 * a character-based string, and an array is a one-dimensioned list of variables.
 */
class finExecVariable
{
public:
    /*! \enum finExecVariableType
     *  \brief The type of variable.
     *
     * This enum declares the types of variable.
     */
    enum finExecVariableType {
        FIN_VR_TYPE_DUMMY = 0,  //!< The dummy variable type. This type is an initialized type of each variable
                                //!< instances when it is created. No variable in script execution procedure is allowed
                                //!< in this type, and an error will report when it happens.

        FIN_VR_TYPE_NULL,       //!< The variable is null, and containes no data.

        FIN_VR_TYPE_NUMERIC,    //!< The variable is a real number.

        FIN_VR_TYPE_STRING,     //!< The variable is a character-based string.

        FIN_VR_TYPE_ARRAY,      //!< The variable is a one-dimensioned list of another variable.

        FIN_VR_TYPE_LINK,       //!< The variable is a symbol link to another variable.

        FIN_VR_TYPE_MAX         //!< An invalid type which only indicates the number of types. It is usually used as a
                                //!< upper bound in allocating a memory space.
    };

protected:
    finExecVariableType _type;
    QString _varName;
    bool _writeProtect;
    bool _leftValue;

    QList<finExecVariable *> _linkedList;
    finExecVariable *_linkTarget;

    double _numVal;
    QString _strVal;
    QList<finExecVariable *> _itemList;
    finExecVariable *_parentVar;

public:
    finExecVariable();
    finExecVariable(const QString &name);
    ~finExecVariable();

    QString getName() const;
    finExecVariableType getType() const;
    bool isWriteProtected() const;
    bool isLeftValue() const;

    finErrorCode setName(const QString &name);
    finErrorCode setType(finExecVariableType type);
    finErrorCode setupWriteProtected(bool blval);
    finErrorCode setWriteProtected();
    finErrorCode clearWriteProtected();
    finErrorCode setupLeftValue(bool blval);
    finErrorCode setLeftValue();
    finErrorCode clearLeftValue();

    double getNumericValue() const;
    QString getStringValue() const;

    finErrorCode setNumericValue(double val);
    finErrorCode setStringValue(const QString &strval);

    int getArrayLength() const;
    finErrorCode preallocArrayLength(int len);
    finExecVariable *getVariableItemAt(int idx) const;
    finExecVariable *getVariableItemAt(int idx);
    finErrorCode clearArrayItems();

    bool isVariableInside(const finExecVariable *var) const;
    bool isInArray() const;
    finExecVariable *getParentVariable() const;
    finErrorCode removeFromArray();

    bool isNumericMatrix(int *rowcnt = NULL, int *colcnt = NULL) const;
    bool isNumericArray(int *cnt = NULL) const;
    bool isStringArray(int *cnt = NULL) const;

    const finExecVariable *getLinkTarget() const;
    finExecVariable *getLinkTarget();
    static finExecVariable *transLinkTarget(finExecVariable *var);
    finErrorCode setLinkTarget(finExecVariable *target);
    finErrorCode unsetLinkTarget();

    finErrorCode readColorValue(QColor *color) const;
    finErrorCode setupColorValue(const QColor &color);

    bool isSameName(const QString &name) const;
    bool isSameValue(finExecVariable *var);

    finErrorCode copyVariableValue(finExecVariable *srcvar);
    finErrorCode copyVariable(finExecVariable *srcvar);

    finErrorCode disposeValue();
    finErrorCode dispose();
    finErrorCode clearLinkedVariables();

    static void releaseNonLeftVariable(finExecVariable *var);

    static finExecVariable *buildNonLeftVariable(finExecVariable *var);
    static finExecVariable *buildCopyLeftVariable(finExecVariable *var);
    static finExecVariable *buildLinkLeftVariable(finExecVariable *var);

    static finExecVariable *buildFuncReturnVariable(finExecVariable *var, finExecEnvironment *env);

    static finErrorCode installSystemVariables(finExecEnvironment *rootenv);

private:
    finErrorCode copyVariableValueIn(finExecVariable *srcvar);
    finErrorCode copyArrayVariable(const finExecVariable *srcvar);
};

typedef enum finExecVariable::finExecVariableType finExecVariableType;

#endif // FINEXECVARIABLE_H
