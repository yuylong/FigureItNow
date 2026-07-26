/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, May 26th, 2016
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */
/*! \file finExecVariable.h
 *  \brief The script variable declaration.
 *
 * This file declares the script runtime variable. A variable is not only the explicit user-defined in their scripts,
 * but also the temporary results of the operator calculations and the function return values.
 */

#ifndef FINEXECVARIABLE_H
#define FINEXECVARIABLE_H


#include <QString>
#include <QImage>
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
    enum Type {
        TP_DUMMY = 0,  //!< The dummy variable type. This type is an initialized type of each variable instances when
                       //!< it is created. No variable in script execution procedure is allowed in this type, and an
                       //!< error will report when it happens.

        TP_NULL,       //!< The variable is null, and containes no data.

        TP_NUMERIC,    //!< The variable is a real number.

        TP_STRING,     //!< The variable is a character-based string.

        TP_IMAGE,      //!< The variable is an pixel-map-based image.

        TP_ARRAY,      //!< The variable is a one-dimensioned list of another variable.

        TP_LINK,       //!< The variable is a symbol link to another variable.

        TP_MAX         //!< An invalid type which only indicates the number of types. It is usually used as a upper
                       //!< bound in allocating a memory space.
    };

protected:
    Type _type;
    QString _varName;
    bool _writeProtect;
    bool _leftValue;

    QList<finExecVariable *> _linkedList;
    finExecVariable *_linkTarget;

    double _numVal;
    QString _strVal;
    QImage _image;
    QList<finExecVariable *> _itemList;
    finExecVariable *_parentVar;

public:
    finExecVariable();
    finExecVariable(const QString &name);
    ~finExecVariable();

    const QString &getName() const;
    Type getType() const;
    bool isWriteProtected() const;
    bool isLeftValue() const;

    void setName(const QString &name);
    void setType(Type type);
    void setupWriteProtected(bool blval);
    void setWriteProtected();
    void clearWriteProtected();
    void setupLeftValue(bool blval);
    void setLeftValue();
    void clearLeftValue();

    double getNumericValue() const;
    QString getStringValue() const;
    QImage getImageValue() const;

    void setNumericValue(double val);
    void setStringValue(const QString &strval);
    void setImageValue(const QImage &img);

    int getArrayLength() const;
    void preallocArrayLength(int len);
    finExecVariable *getVariableItemAt(int idx) const;
    finExecVariable *getVariableItemAt(int idx);
    void clearArrayItems();

    bool isVariableInside(const finExecVariable *var) const;
    bool isInArray() const;
    finExecVariable *getParentVariable() const;
    void removeFromArray();

    bool isNumericMatrix(int *rowcnt = nullptr, int *colcnt = nullptr) const;
    bool isNumericArray(int *cnt = nullptr) const;
    bool isStringArray(int *cnt = nullptr) const;
    int maxArrayLevel() const;
    bool hasMultiLevel() const;

    const finExecVariable *getLinkTarget() const;
    finExecVariable *getLinkTarget();
    static finExecVariable *transLinkTarget(finExecVariable *var);
    void setLinkTarget(finExecVariable *target);
    void unsetLinkTarget();

    void readBoolValue(bool *blval) const;
    void setupBoolValue(bool blval);
    void readColorValue(QColor *color) const;
    void setupColorValue(const QColor &color);

    bool isSameName(const QString &name) const;
    bool isSameValue(finExecVariable *var);

    void copyVariableValue(finExecVariable *srcvar);
    void smartCopyVariableValue(finExecVariable *srcvar);
    void copyVariable(finExecVariable *srcvar);

    void disposeValue();
    void dispose();
    void clearLinkedVariables();

    static void transToPointList(finExecVariable *var, QList<QPointF> *ptlist);
    static void transToPointList(finExecVariable *xvar, finExecVariable *yvar, QList<QPointF> *ptlist);

    static void releaseNonLeftVariable(finExecVariable *var);

    static finExecVariable *buildNonLeftVariable(finExecVariable *var);
    static finExecVariable *buildCopyLeftVariable(finExecVariable *var);
    static finExecVariable *buildLinkLeftVariable(finExecVariable *var);

    static finExecVariable *buildFuncReturnVariable(finExecVariable *var, finExecEnvironment *env);

    static void installSystemVariables(finExecEnvironment *rootenv);

private:
    void copyVariableValueIn(finExecVariable *srcvar);
    void copyArrayVariable(const finExecVariable *srcvar);

    static void transToPointListArray(finExecVariable *aryvar, QList<QPointF> *ptlist);
    static void transToPointListMatrix(finExecVariable *matvar, QList<QPointF> *ptlist);
};

typedef enum finExecVariable::Type finExecVariableType;

#endif // FINEXECVARIABLE_H
