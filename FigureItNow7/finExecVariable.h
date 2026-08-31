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
    Type _type;                         //!< Current runtime value type.
    QString _varName;                   //!< Script-visible variable name.
    bool _writeProtect;                 //!< Whether mutation is prohibited.
    bool _leftValue;                    //!< Whether the value may be assigned to.

    QList<finExecVariable *> _linkedList;  //!< Links that currently refer to this variable.
    finExecVariable *_linkTarget;          //!< Target variable when the type is TP_LINK.

    double _numVal;                        //!< Numeric payload for TP_NUMERIC.
    QString _strVal;                       //!< String payload for TP_STRING.
    QImage _image;                         //!< Image payload for TP_IMAGE.
    QList<finExecVariable *> _itemList;    //!< Owned child values for TP_ARRAY.
    finExecVariable *_parentVar;           //!< Containing array variable, if any.

public:
    /*! \brief Constructs an unnamed null variable. */
    finExecVariable();

    /*! \brief Constructs a null variable with the given script-visible name. */
    finExecVariable(const QString &name);

    /*! \brief Detaches links and array ownership, then releases the contained value. */
    ~finExecVariable();

    /*! \name Metadata And Access Mode
     *  \brief Inspect and configure the variable name, type, and assignment permissions.
     */
    ///@{
    /*! \brief Returns the script-visible name. */
    const QString &getName() const;
    /*! \brief Returns the current runtime value type. */
    Type getType() const;
    /*! \brief Returns whether this variable and its array children are write-protected. */
    bool isWriteProtected() const;
    /*! \brief Returns whether this variable can be used as an assignment target. */
    bool isLeftValue() const;

    /*! \brief Sets the script-visible name. */
    void setName(const QString &name);
    /*! \brief Sets the type of a null variable. */
    void setType(Type type);
    /*! \brief Sets write protection recursively for this value and its array children. */
    void setupWriteProtected(bool blval);
    /*! \brief Enables write protection recursively. */
    void setWriteProtected();
    /*! \brief Disables write protection recursively. */
    void clearWriteProtected();
    /*! \brief Sets left-value state recursively for this value and its array children. */
    void setupLeftValue(bool blval);
    /*! \brief Marks this value and its array children as assignable. */
    void setLeftValue();
    /*! \brief Marks this value and its array children as non-assignable. */
    void clearLeftValue();
    ///@}

    /*! \name Scalar Values
     *  \brief Read and write numeric, string, and image payloads.
     */
    ///@{
    /*! \brief Returns the numeric payload, or 0.0 for another type. */
    double getNumericValue() const;
    /*! \brief Returns the string payload, or an empty string for another type. */
    QString getStringValue() const;
    /*! \brief Returns the image payload, or a null image for another type. */
    QImage getImageValue() const;

    /*! \brief Sets the numeric payload, initializing a null variable as numeric. */
    void setNumericValue(double val);
    /*! \brief Sets the string payload, initializing a null variable as a string. */
    void setStringValue(const QString &strval);
    /*! \brief Sets the image payload, initializing a null variable as an image. */
    void setImageValue(const QImage &img);
    ///@}

    /*! \name Array Management
     *  \brief Create, access, inspect, and detach owned array items.
     */
    ///@{
    /*! \brief Returns the number of items, or zero when this is not an array. */
    int getArrayLength() const;
    /*! \brief Grows an array to at least \a len items. */
    void preallocArrayLength(int len);
    /*! \brief Returns an existing array item without extending the array. */
    finExecVariable *getVariableItemAt(int idx) const;
    /*! \brief Returns an array item, extending a null or array value as required. */
    finExecVariable *getVariableItemAt(int idx);
    /*! \brief Deletes all array items and leaves the variable as an empty array. */
    void clearArrayItems();

    /*! \brief Returns whether \a var is this variable or one of its array descendants. */
    bool isVariableInside(const finExecVariable *var) const;
    /*! \brief Returns whether this variable currently belongs to an array. */
    bool isInArray() const;
    /*! \brief Returns the containing array variable, or \c nullptr. */
    finExecVariable *getParentVariable() const;
    /*! \brief Detaches this variable from its containing array without deleting it. */
    void removeFromArray();

    /*! \brief Tests whether this value is a rectangular numeric matrix. */
    bool isNumericMatrix(int *rowcnt = nullptr, int *colcnt = nullptr) const;
    /*! \brief Tests whether every direct array item is numeric. */
    bool isNumericArray(int *cnt = nullptr) const;
    /*! \brief Tests whether every direct array item is a string. */
    bool isStringArray(int *cnt = nullptr) const;
    /*! \brief Returns the greatest nesting depth of contained arrays. */
    int maxArrayLevel() const;
    /*! \brief Returns whether any direct array item is itself an array. */
    bool hasMultiLevel() const;
    ///@}

    /*! \name Link Values
     *  \brief Resolve and maintain symbolic links to other runtime variables.
     */
    ///@{
    /*! \brief Resolves this link chain and returns its final target. */
    const finExecVariable *getLinkTarget() const;
    /*! \brief Resolves this link chain and returns its final target. */
    finExecVariable *getLinkTarget();
    /*! \brief Resolves \a var when it is a link; preserves null input. */
    static finExecVariable *transLinkTarget(finExecVariable *var);
    /*! \brief Sets this variable to link to \a target. */
    void setLinkTarget(finExecVariable *target);
    /*! \brief Removes the current link target and its back-reference. */
    void unsetLinkTarget();
    ///@}

    /*! \name Value Conversion And Comparison
     *  \brief Convert variables to logical or color values and compare their contents.
     */
    ///@{
    /*! \brief Converts this value to a FIN-script logical value. */
    void readBoolValue(bool *blval) const;
    /*! \brief Stores a logical value as the numeric values 0.0 or 1.0. */
    void setupBoolValue(bool blval);
    /*! \brief Reads a three- or four-component numeric array as a QColor. */
    void readColorValue(QColor *color) const;
    /*! \brief Initializes this null variable as an RGBA numeric array. */
    void setupColorValue(const QColor &color);

    /*! \brief Compares the script-visible name with \a name. */
    bool isSameName(const QString &name) const;
    /*! \brief Compares this value with \a var, resolving links recursively. */
    bool isSameValue(finExecVariable *var);
    ///@}

    /*! \name Copy And Lifetime Management
     *  \brief Copy variable content and dispose values, links, and ownership relationships.
     */
    ///@{
    /*! \brief Replaces this value with a deep copy of \a srcvar. */
    void copyVariableValue(finExecVariable *srcvar);
    /*! \brief Copies \a srcvar, transferring eligible temporary array contents. */
    void smartCopyVariableValue(finExecVariable *srcvar);
    /*! \brief Copies both the value and metadata from \a srcvar. */
    void copyVariable(finExecVariable *srcvar);

    /*! \brief Releases the current payload and resets the type to TP_NULL. */
    void disposeValue();
    /*! \brief Releases the payload and resets name and access-mode metadata. */
    void dispose();
    /*! \brief Detaches every variable that links to this variable. */
    void clearLinkedVariables();
    ///@}

    /*! \name Point And Ownership Helpers
     *  \brief Convert numeric arrays to point lists and construct values for execution contexts.
     */
    ///@{
    /*! \brief Converts a numeric point array or matrix into \a ptlist. */
    static void transToPointList(finExecVariable *var, QList<QPointF> *ptlist);
    /*! \brief Converts paired numeric coordinate arrays into \a ptlist. */
    static void transToPointList(finExecVariable *xvar, finExecVariable *yvar, QList<QPointF> *ptlist);

    /*! \brief Deletes \a var when it is a non-left temporary value. */
    static void releaseNonLeftVariable(finExecVariable *var);

    /*! \brief Returns \a var as a detached, non-left, write-protected value. */
    static finExecVariable *buildNonLeftVariable(finExecVariable *var);
    /*! \brief Returns an assignable copy of \a var. */
    static finExecVariable *buildCopyLeftVariable(finExecVariable *var);
    /*! \brief Returns an assignable link or copy suitable for variable binding. */
    static finExecVariable *buildLinkLeftVariable(finExecVariable *var);

    /*! \brief Prepares a value for transfer out of a function environment. */
    static finExecVariable *buildFuncReturnVariable(finExecVariable *var, finExecEnvironment *env);

    /*! \brief Installs predefined system variables into the root environment. */
    static void installSystemVariables(finExecEnvironment *rootenv);
    ///@}

private:
    /*! \brief Copies only the payload and type from \a srcvar. */
    void copyVariableValueIn(finExecVariable *srcvar);
    /*! \brief Deep-copies the array payload from \a srcvar. */
    void copyArrayVariable(const finExecVariable *srcvar);

    /*! \brief Converts a flat numeric array into successive point pairs. */
    static void transToPointListArray(finExecVariable *aryvar, QList<QPointF> *ptlist);
    /*! \brief Converts rows of a numeric matrix into points. */
    static void transToPointListMatrix(finExecVariable *matvar, QList<QPointF> *ptlist);
};

typedef enum finExecVariable::Type finExecVariableType;

#endif // FINEXECVARIABLE_H
