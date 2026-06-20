/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, May 26th, 2016
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */
/*! \file finErrorCode.h
 *  \brief Declarations of error codes, exceptions, and diagnostic utilities.
 *
 *  This header defines the error-code enumeration shared by every FIN-7 module, the finException
 *  type used to propagate errors across module boundaries, and the convenience macros used for
 *  throwing and logging.
 */

#ifndef FINERRORCODE_H
#define FINERRORCODE_H

#include <QObject>
#include <QDebug>
#include <QException>
#include <QString>

/*! \class finErrorKits
 *  \brief Global error-code definitions and lookup utilities.
 *
 *  finErrorKits is used as a namespace. It defines the ErrorCode enumeration shared by every FIN-7 module
 *  and provides the static helpers used to classify and render an error code. The class is never
 *  instantiated; all of its members are static.
 */
class finErrorKits
{
public:
    /*! \enum finErrorKits::ErrorCode
     *  \brief Error codes returned by FIN-7 functions.
     *
     *  Every FIN-7 function that can fail returns an ErrorCode value. Codes fall into three categories:
     *
     *  - Success codes (\c >= 0) indicate the operation completed and a result is available. A non-zero
     *    success code carries extra information (for example, a warning was raised) but the caller can still
     *    treat the operation as successful.
     *  - Warning codes (\c > 0) are positive and indicate that something noteworthy happened that the
     *    caller may want to inspect.
     *  - Failure codes (\c < 0) indicate the operation failed. No usable result is produced and the
     *    caller should propagate the error.
     *
     *  \see isErrorResult
     *  \see errcodeToString
     */
    enum ErrorCode {
        EC_SUCCESS       =     0,  //!< Operation succeeded; no error occurred.

        EC_DUPLICATE_OP  =     1,  //!< The operation has already been performed; the current call is redundant.

        EC_REACH_BOTTOM  =     2,  //!< The input stream has been fully consumed; there is nothing left to read.

        EC_UNREADY_WARN  =    10,  //!< An operation was performed on an object that is not yet ready,
                                   //!< but this did not cause a failure.

        EC_PRECISE_LOSS  =    21,  //!< The operation succeeded, but the result has lost precision
                                   //!< beyond what is normally acceptable.

        EC_NORMAL_WARN   =    55,  //!< The operation succeeded with an unspecified warning.

        EC_NULL_POINTER  =    -1,  //!< An input or argument was a null pointer.

        EC_FILE_NOT_OPEN =    -2,  //!< The required file is not open, so its contents cannot be read.

        EC_OUT_OF_MEMORY =    -3,  //!< The operation ran out of memory.

        EC_OVERFLOW      =    -4,  //!< The calculation result cannot be represented on this system
                                   //!< because the target type is too narrow.

        EC_NOT_FOUND     =    -7,  //!< The requested object was not found.

        EC_READ_ERROR    =    -8,  //!< The input is malformed; no further checking is required.

        EC_CONTENTION    =    -9,  //!< The requested slot (position or key) is already in use.

        EC_INVALID_PARAM =   -12,  //!< The parameter is not supported by this overload or call site.

        EC_STATE_ERROR   =   -33,  //!< The operation was attempted in an invalid state.

        EC_UNKNOWN_ERROR = -9099,  //!< An unknown error occurred.

        EC_NON_IMPLEMENT = -9999,  //!< The function is not yet implemented;
                                   //!< this code should not appear in a release build.

        EC_DUMMY         = 0xFFFF  //!< Sentinel value used as a table terminator and to signal a
                                   //!< fatal internal error; should never appear under normal
                                   //!< conditions.
    };

public:
    /*!
     *  \brief Default constructor (unused).
     *
     *  finErrorKits is a namespace-like utility class and is not meant to be instantiated. The constructor
     *  is kept public only to allow the translation unit to compile; it does nothing.
     */
    finErrorKits();

    /*!
     *  \brief Returns the human-readable description of an error code.
     *
     *  Looks up the description string registered for \a errcode and returns a pointer to it. The pointer
     *  is valid for the lifetime of the program.
     *
     *  \param errcode  The error code to render.
     *  \return Pointer to the description string, or a pointer to a default "Unknown error" string if the
     *          code is not registered.
     *
     *  \see ErrorCode
     */
    static const QString *errcodeToString(enum ErrorCode errcode);

    /*!
     *  \brief Checks whether an error code represents a failure.
     *
     *  \param errcode  The error code to inspect.
     *  \return \c true if \a errcode is a failure code (negative); \c false if it represents a success
     *          (including warnings).
     *
     *  \see ErrorCode
     */
    static bool isErrorResult(enum ErrorCode errcode);

private:
    static QString _defErrorString;
};

/*! \typedef finErrorCode
 *  \brief Shorthand alias for finErrorKits::ErrorCode.
 *
 *  Used throughout the FIN-7 codebase in place of the fully qualified finErrorKits::ErrorCode.
 */
typedef enum finErrorKits::ErrorCode finErrorCode;

/*! \class finExceptionObject
 *  \brief Mix-in for objects that can be attached to a finException.
 *
 *  Classes that should be reachable from a thrown finException (typically the receiver of a finThrowObj()
 *  call) inherit from this interface and implement dumpObjInfo() to produce a one-line textual description.
 *
 *  \see finException
 *  \see finThrowObj
 */
class finExceptionObject
{
public:
    /*!
     *  \brief Returns a one-line textual description of this object.
     *
     *  The result is embedded in the log line produced by finException::dump() whenever this object is
     *  attached to an exception.
     */
    virtual QString dumpObjInfo() const = 0;

    /*!
     *  \brief QDebug stream operator that delegates to dumpObjInfo().
     */
    friend QDebug &operator<<(QDebug &debug, const finExceptionObject &obj);
};

/*! \class finException
 *  \brief Exception type carrying an error code, description, and source location.
 *
 *  finException extends QException so it can travel across Qt's event-loop boundaries. It records the
 *  originating error code, an optional human-readable description, the source file / function / line at
 *  the throw site, a severity level, and an optional reference to a finExceptionObject that carries
 *  extra context.
 *
 *  On construction the exception calls dump(), which logs itself at the chosen severity level. The
 *  exception is then re-thrown by the caller (or by raise() / clone() as required by QException).
 *
 *  \see finErrorCode
 *  \see finExceptionObject
 */
class finException : QException
{
public:
    /*! \enum finException::Level
     *  \brief Severity level attached to a finException.
     *
     *  The level controls which qDebug / qInfo / qWarning / qCritical / qFatal stream the constructor's
     *  dump() call writes to, and must lie inside the range [EL_DUMMY, EL_MAX).
     */
    enum Level {
        EL_DUMMY = 0,    //!< Reserved sentinel; not a real level.
        EL_DEBUG,        //!< Debug-level diagnostic; written to qDebug().
        EL_INFO,         //!< Informational message; written to qInfo().
        EL_WARNING,      //!< Warning; written to qWarning().
        EL_CRITIAL,      //!< Critical error; written to qCritical().
        EL_FATAL,        //!< Fatal error; written to qFatal() (may abort).
        EL_MAX           //!< Sentinel marking the end of the enum.
    };

    /*!
     *  \brief Returns the lowercase name of a level, used in log output.
     *
     *  \param level  The level to look up.
     *  \return A reference to a static string such as "debug", "warn", or "fatal". The pointer is valid
     *          for the lifetime of the program.
     */
    static const QString &levelName(Level level);

private:
    finErrorCode _errCode;              //!< The originating error code.
    QString _errDesc;                   //!< Optional human-readable description.
    QString _srcFile;                   //!< Source file where the exception was thrown.
    QString _srcPrettyFunction;         //!< Pretty-printed function name of the throw site.
    unsigned long _srcLine;             //!< Line number of the throw site.
    Level _level;                       //!< Severity level.
    finExceptionObject *_errObj;        //!< Optional object that carries extra context, or \c nullptr.

public:
    /*!
     *  \brief Constructs a finException with no description or source location.
     *
     *  The level defaults to EL_CRITIAL. The exception is logged immediately by dump().
     */
    finException(finErrorCode errcode, Level level = EL_CRITIAL, finExceptionObject *errobj = nullptr);

    /*!
     *  \brief Constructs a finException with a description but no source location.
     */
    finException(finErrorCode errcode, const QString &errdesc,
                 Level level = EL_CRITIAL, finExceptionObject *errobj = nullptr);

    /*!
     *  \brief Constructs a finException with a description and source location.
     *
     *  This is the constructor used by the finThrow* macros; the source location is captured automatically
     *  by the preprocessor.
     */
    finException(finErrorCode errcode, const QString &errdesc,
                 const QString &srcfile, const QString &srcfunc, unsigned long srcline,
                 Level level = EL_CRITIAL, finExceptionObject *errobj = nullptr);

    /*!
     *  \brief Copy constructor.
     */
    finException(const finException &e);

    /*!
     *  \brief Returns the error code carried by this exception.
     */
    finErrorCode getErrorCode() const;

    /*!
     *  \brief Returns the human-readable description, or an empty string if none was set.
     */
    const QString &getErrorDescription() const;

    /*!
     *  \brief Returns the extra-context object, or \c nullptr if none was attached.
     */
    finExceptionObject *getErrorObject() const;

    /*!
     *  \brief Re-throws this exception. Required by QException.
     */
    void raise() const override;

    /*!
     *  \brief Returns a heap-allocated copy of this exception. Required by QException.
     */
    finException *clone() const override;

    /*!
     *  \brief Builds a single-line textual description of the exception.
     *
     *  The output includes the source location, the error code, the description, and (if present) the
     *  dumpObjInfo() of the attached finExceptionObject.
     */
    QString dumpInfo() const;

    /*!
     *  \brief Logs the exception at its level via the matching qDebug / qInfo / qWarning / qCritical /
     *         qFatal channel.
     */
    void dump() const;
};

/*! \def _FIN_DEBUGHEAD(func,file,line,level)
 *  \brief Builds the "[func (file:line) level]" prefix used in log lines.
 *
 *  Internal helper for the finDebug* macros and finException::dumpInfo().
 */
#define _FIN_DEBUGHEAD(func,file,line,level)  \
    (QString("[%1 (%2:%3) %4]").arg(func).arg(file).arg(line).arg(level))

/*! \def _FIN_DUMPOBJ(pobj)
 *  \brief Wraps an object's dumpObjInfo() output in braces.
 *
 *  Internal helper for embedding a finExceptionObject into a log line.
 */
#define _FIN_DUMPOBJ(pobj)  QString("{%1}").arg((pobj)->dumpObjInfo())

/*! \def finThrow(errcode, errdesc)
 *  \brief Throws a finException at the current source location.
 *
 *  Captures __FILE__, __PRETTY_FUNCTION__, and __LINE__ automatically and logs the exception at the
 *  EL_CRITIAL level before the throw propagates.
 */
#define finThrow(errcode, errdesc)  do { \
        throw finException((errcode), (errdesc), __FILE__, __PRETTY_FUNCTION__, __LINE__); \
    } while (0)

/*! \def finThrowObj(errcode, errdesc)
 *  \brief Like finThrow(), but attaches \c this (must derive from finExceptionObject) to the exception.
 */
#define finThrowObj(errcode, errdesc)  do { \
        throw finException((errcode), (errdesc), __FILE__, __PRETTY_FUNCTION__, __LINE__, \
                           finException::EL_CRITIAL, this); \
    } while (0)

/*! \def finThrowFatal(errcode, errdesc)
 *  \brief Like finThrow(), but logs at the EL_FATAL level.
 */
#define finThrowFatal(errcode, errdesc)  do { \
        throw finException((errcode), (errdesc), __FILE__, __PRETTY_FUNCTION__, __LINE__, \
                           finException::EL_FATAL); \
    } while (0)

/*! \def finThrowFatalObj(errcode, errdesc)
 *  \brief Like finThrowObj(), but logs at the EL_FATAL level.
 */
#define finThrowFatalObj(errcode, errdesc)  do { \
        throw finException((errcode), (errdesc), __FILE__, __PRETTY_FUNCTION__, __LINE__, \
                           finException::EL_FATAL, this); \
    } while (0)

/*! \def finDebugHead(level)
 *  \brief Builds the source-location prefix for a finDebug* log stream.
 */
#define finDebugHead(level)  _FIN_DEBUGHEAD(__PRETTY_FUNCTION__, __FILE__, __LINE__, level)

/*! \def finDebug
 *  \brief Stream-style qDebug() prefixed with the current source location.
 */
#define finDebug    (qDebug().noquote() << finDebugHead("debug"))

/*! \def finInfo
 *  \brief Stream-style qInfo() prefixed with the current source location.
 */
#define finInfo     (qInfo().noquote() << finDebugHead("info"))

/*! \def finWarning
 *  \brief Stream-style qWarning() prefixed with the current source location.
 */
#define finWarning  (qWarning().noquote() << finDebugHead("warn"))

/*! \def finCritical
 *  \brief Stream-style qCritical() prefixed with the current source location.
 */
#define finCritical (qCritical().noquote() << finDebugHead("critial"))

/*! \def finFatal
 *  \brief Stream-style qFatal() prefixed with the current source location.
 *
 *  qFatal() aborts the process by default; use sparingly.
 */
#define finFatal    (qFatal().noquote() << finDebugHead("fatal"))

/*! \def finDbgObj
 *  \brief Inserts a brace-wrapped dumpObjInfo() of \c this into a log stream.
 *
 *  Intended to be used inside a finDebug* stream expression in a class that derives from finExceptionObject.
 */
#define finDbgObj   _FIN_DUMPOBJ(this)

#endif // FINERRORCODE_H
