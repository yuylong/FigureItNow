/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, May 26th, 2016
 * Copyright(c) 2015-2024 Yulong Yu. All rights reserved.
 */
/*! \file finErrorCode.h
 *  \brief The declarations of the Error Codes.
 *
 * This file includes all the declaration and the inline defintions about the Error codes used by the FIN-7 system.
 */

#ifndef FINERRORCODE_H
#define FINERRORCODE_H

#include <QObject>
#include <QDebug>
#include <QException>
#include <QString>

/*! \class finErrorKits
 *  \brief The globally definitions and the tools of Error Codes
 *
 * This class is used as a namespace including all the definitions and the tools about the error codes used in the
 * FIN-7 system. You should not instantiate this class, and there is no private member to store any anything. You shall
 * instantiate the enum inside this class to store the Error Codes, as well as all the tools are provided as static
 * functions that you can invoke directly.
 */
class finErrorKits
{
public:
    /*! \enum ErrorCode
     *  \brief The Error Code definitions
     *
     * All the definitions of the Error Codes used by the FIN-7 system is here. Other modules use its instances to
     * store the Error Codes. The Error codes have three categories, SUCCESS, WARNING and FAILURE, where SUCCESS means
     * the operation is finished without any problem, and the result is gotten; WARNING means the result is achieved
     * while there is something distrubed the process; FAILURE means the operation failed and no result returns.
     */
    enum ErrorCode {
        EC_SUCCESS       =     0,  //!< Execution Success, no error happened.

        EC_DUPLICATE_OP  =     1,  //!< The same operation has already done, currently invocation is an redundant call.

        EC_REACH_BOTTOM  =     2,  //!< The input stream is all flushed.

        EC_UNREADY_WARN  =    10,  //!< Some operation is taken on an unready object, but it will not cause an error.

        EC_PRECISE_LOSS  =    21,  //!< Execution Success, but the result is not very precise, some error is
                                   //!< introduced into the result, but the error is accepted.

        EC_NORMAL_WARN   =    55,  //!< Execution Success, but some warnings are there. The warning is not a specific
                                   //!< type.

        EC_NULL_POINTER  =    -1,  //!< The inputs or arguments contain null pointer.

        EC_FILE_NOT_OPEN =    -2,  //!< The required file is not open, the data cannot be read.

        EC_OUT_OF_MEMORY =    -3,  //!< There is no memory left for the given operation.

        EC_OVERFLOW      =    -4,  //!< The calculation results cannot be presented in current computer system,
                                   //!< because the data structure bit width is too narrow.

        EC_NOT_FOUND     =    -7,  //!< The target object is not found.

        EC_READ_ERROR    =    -8,  //!< The input string is error, no more check is necessary.

        EC_CONTENTION    =    -9,  //!< The insertion operation cannot taken because the position or the index key is
                                   //!< occupied by others.

        EC_INVALID_PARAM =   -12,  //!< The parameter is unsupported in the certain invocation.

        EC_STATE_ERROR   =   -33,  //!< The operation is taken at a wrong state.

        EC_UNKNOWN_ERROR = -9099,  //!< Unkonwn error.

        EC_NON_IMPLEMENT = -9999,  //!< The function is not implemented currently, it should not appear in the
                                   //!< released code.

        EC_DUMMY         = 0xFFFF  //!< An invalid error code value. Some serious fetal internal error will lead this
                                   //!< error code, and should not appear in a normal case.
    };

public:
    /*! \fn finErrorKits
     * \brief The constructor of class finErrorKits.
     *
     * This function should not be called due to its should not build an instance of class finErrorKits. All the
     * methods in finErrorKits is static, and no instance is necessary.
     */
    finErrorKits();

    /*! \fn errcodeToString
     *  \brief Convert a error code value to a readable string.
     *
     * This function converts the given error code to a readable string, which is used to output the error to the user
     * interface to tell the user what happens inside the FIN-7 system.
     *
     * \param errcode
     *        The give error code.
     * \return The readable string corresponding to the given error code.
     *
     * \see ErrorCode
     */
    static const QString *errcodeToString(enum ErrorCode errcode);

    /*! \fn isErrorResult
     *  \brief The given error code represents an error or just a warning.
     *
     * This function is used to judge the given error code is an error which will affect the next step executing, or
     * just a warning which will not affect the executing.
     *
     * \param errcode
     *        The given error code.
     * \return The judgement result. TRUE if the given error code reperesents an error, else a FALSE is returned.
     *
     * \see ErrorCode
     */
    static bool isErrorResult(enum ErrorCode errcode);

private:
    static QString _defErrorString;
};

/*! \typedef finErrorCode
 *  \brief A mirror declaration of finErrorKits::ErrorCode for short.
 *
 * The other part of code in FIN-7 use this short type-define instead of directly use
 * finErrorKits::ErrorCode for short.
 */
typedef enum finErrorKits::ErrorCode finErrorCode;

class finExceptionObject
{
public:
    virtual QString dumpObjInfo() const = 0;
    friend QDebug operator<<(QDebug debug, const finExceptionObject &obj);
};

class finException : QException
{
private:
    finErrorCode _errCode;
    QString _errDesc;
    finExceptionObject *_errObj;

public:
    finException(finErrorCode errcode, QString errdesc = "", finExceptionObject *errobj = nullptr);
    finException(const finException &e);

    finErrorCode getErrorCode() const;
    const QString &getErrorDescription() const;

    void raise() const override;
    finException *clone() const override;
};

#endif // FINERRORCODE_H
