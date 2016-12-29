/*! \file finErrorCode.h
 *  \brief The declarations of the Error Codes.
 *
 * This file includes all the declaration and the inline defintions about the Error codes used by the FIN-7 system.
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
 * 20151101    0  Yulong Yu    Create this file.
 * 20160526    1  Yulong Yu    Add the doxygen-styled comments.
 * 20161223    2  Yulong Yu    Refine the comment styles and the copyright license.
 */

#ifndef FINERRORCODE_H
#define FINERRORCODE_H

#include <QObject>
#include <QString>


/*! \class finErrorCodeKits
 *  \brief The globally definitions and the tools of Error Codes
 *
 * This class is used as a namespace including all the definitions and the tools about the error codes used in the
 * FIN-7 system. You should not instantiate this class, and there is no private member to store any anything. You shall
 * instantiate the enum inside this class to store the Error Codes, as well as all the tools are provided as static
 * functions that you can invoke directly.
 */
class finErrorCodeKits
{
public:
    /*! \enum finErrorCodeType
     *  \brief The Error Code definitions
     *
     * All the definitions of the Error Codes used by the FIN-7 system is here. Other modules use its instances to
     * store the Error Codes. The Error codes have three categories, SUCCESS, WARNING and FAILURE, where SUCCESS means
     * the operation is finished without any problem, and the result is gotten; WARNING means the result is achieved
     * while there is something distrubed the process; FAILURE means the operation failed and no result returns.
     */
    enum finErrorCodeType {
        FIN_EC_SUCCESS       =     0,  //!< Execution Success, no error happened.

        FIN_EC_DUPLICATE_OP  =     1,  //!< The same operation has already done, currently invocation is an redundant
                                       //!< call.

        FIN_EC_REACH_BOTTOM  =     2,  //!< The input stream is all flushed.

        FIN_EC_UNREADY_WARN  =    10,  //!< Some operation is taken on an unready object, but it will not cause an
                                       //!< error.

        FIN_EC_PRECISE_LOSS  =    21,  //!< Execution Success, but the result is not very precise, some error is
                                       //!< introduced into the result, but the error is accepted.

        FIN_EC_NORMAL_WARN   =    55,  //!< Execution Success, but some warnings are there. The warning is not a
                                       //!< specific type.

        FIN_EC_NULL_POINTER  =    -1,  //!< The inputs or arguments contain null pointer.

        FIN_EC_FILE_NOT_OPEN =    -2,  //!< The required file is not open, the data cannot be read.

        FIN_EC_OUT_OF_MEMORY =    -3,  //!< There is no memory left for the given operation.

        FIN_EC_OVERFLOW      =    -4,  //!< The calculation results cannot be presented in current computer system,
                                       //!< because the data structure bit width is too narrow.

        FIN_EC_NOT_FOUND     =    -7,  //!< The target object is not found.

        FIN_EC_READ_ERROR    =    -8,  //!< The input string is error, no more check is necessary.

        FIN_EC_CONTENTION    =    -9,  //!< The insertion operation cannot taken because the position or the index key
                                       //!< is occupied by others.

        FIN_EC_INVALID_PARAM =   -12,  //!< The parameter is unsupported in the certain invocation.

        FIN_EC_STATE_ERROR   =   -33,  //!< The operation is taken at a wrong state.

        FIN_EC_UNKNOWN_ERROR = -9099,  //!< Unkonwn error.

        FIN_EC_NON_IMPLEMENT = -9999,  //!< The function is not implemented currently, it should not appear in the
                                       //!< released code

        FIN_EC_DUMMY         = 0xFFFF  //!< An invalid error code value. Some serious fetal internal error will lead
                                       //!< this error code, and should not appear in a normal case.
    };

public:
    /*! \fn finErrorCodeKits
     * \brief The constructor of class finErrorCodeKits.
     *
     * This function should not be called due to its should not build an instance of class finErrorCodeKits. All the
     * methods in finErrorCodeKits is static, and no instance is necessary.
     */
    finErrorCodeKits();

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
     * \see finErrorCodeType
     */
    static const QString *errcodeToString(enum finErrorCodeType errcode);

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
     * \see finErrorCodeType
     */
    static bool isErrorResult(enum finErrorCodeType errcode);
};

/*! \typedef finErrorCode
 *  \brief A mirror declaration of finErrorCodeType for short.
 *
 * The other part of code in FIN-7 use this short type-define instead of directly use
 * finErrorCodeKits::finErrorCodeType for short.
 */
typedef enum finErrorCodeKits::finErrorCodeType finErrorCode;

#endif // FINERRORCODE_H
