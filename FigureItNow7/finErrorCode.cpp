/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2024 Yulong Yu. All rights reserved.
 */

#include "finErrorCode.h"

#include <QtGlobal>
#include <QObject>
#include <QDebug>

static const struct {
    finErrorCode _errCode;
    const QString _errString;
} _finErrorCodeStringMap[] =
{
    // Success
    { finErrorKits::EC_SUCCESS,
        QObject::tr("No error")                                  },

    // Warnings
    { finErrorKits::EC_DUPLICATE_OP,
        QObject::tr("Duplicated operations")                     },
    { finErrorKits::EC_REACH_BOTTOM,
        QObject::tr("Reach accessing bottom")                    },
    { finErrorKits::EC_UNREADY_WARN,
        QObject::tr("Unready object detected")                   },
    { finErrorKits::EC_PRECISE_LOSS,
        QObject::tr("Success with precise loss")                 },
    { finErrorKits::EC_NORMAL_WARN,
        QObject::tr("Success with warning")                      },

    // Errors
    { finErrorKits::EC_NULL_POINTER,
        QObject::tr("Access to null pointer")                    },
    { finErrorKits::EC_FILE_NOT_OPEN,
        QObject::tr("File does not open")                        },
    { finErrorKits::EC_OUT_OF_MEMORY,
        QObject::tr("Out of memory")                             },
    { finErrorKits::EC_OVERFLOW,
        QObject::tr("Calculation overflow")                      },
    { finErrorKits::EC_NOT_FOUND,
        QObject::tr("Object not found")                          },
    { finErrorKits::EC_READ_ERROR,
        QObject::tr("Reading object failure")                    },
    { finErrorKits::EC_CONTENTION,
        QObject::tr("Requested position is occupied.")           },
    { finErrorKits::EC_INVALID_PARAM,
        QObject::tr("Invalid Parameter")                         },
    { finErrorKits::EC_STATE_ERROR,
        QObject::tr("Operation on a wrong state")                },
    { finErrorKits::EC_UNKNOWN_ERROR,
        QObject::tr("Unkown error")                              },
    { finErrorKits::EC_NON_IMPLEMENT,
        QObject::tr("Non-implement")                             },

    // Dummy End
    { finErrorKits::EC_DUMMY,
        QString("")                                              }
};

QString finErrorKits::_defErrorString = QObject::tr("Unknown error");

finErrorKits::finErrorKits()
{
    // We keep this function public, not private or protected, allowing the upper layered code create the instance of
    // this class. But it is not reaaly necessary, because currently all the functions in this class is static.
    return;
}

const QString *
finErrorKits::errcodeToString(enum ErrorCode errcode)
{
    int i = 0;

    // Iterate all the error codes, and return the corresponding error description string defined in
    // _finErrorCodeStringMap.
    while (true) {
        if (_finErrorCodeStringMap[i]._errCode == finErrorKits::EC_DUMMY)
            break;

        if (_finErrorCodeStringMap[i]._errCode == errcode)
            return &_finErrorCodeStringMap[i]._errString;
        i++;
    }
    return &_defErrorString;
}

bool
finErrorKits::isErrorResult(enum ErrorCode errcode)
{
    // Any error code less than 0 is recognized as an error.
    return (errcode < 0);
}

QDebug &operator<<(QDebug &debug, const finExceptionObject &obj)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << obj.dumpObjInfo();
    return debug;
}

const QString &finException::levelName(Level level)
{
    static QString levelname[EL_MAX] = {
        QString(), QString("debug"), QString("info"), QString("warn"), QString("critial"), QString("fatal"),
    };

    Q_ASSERT_X(level > EL_DUMMY && level < EL_MAX, "cast", "invalid level");
    return levelname[static_cast<unsigned>(level)];
}

finException::finException(finErrorCode errcode, finException::Level level, finExceptionObject *errobj)
    : _errCode(errcode), _errDesc(), _srcFile(), _srcPrettyFunction(), _srcLine(0), _level(level), _errObj(errobj)
{
    this->dump();
}

finException::finException(finErrorCode errcode, const QString &errdesc,
                           finException::Level level, finExceptionObject *errobj)
    : _errCode(errcode), _errDesc(errdesc), _srcFile(), _srcPrettyFunction(), _srcLine(0),
    _level(level), _errObj(errobj)
{
    this->dump();
}

finException::finException(finErrorCode errcode, const QString &errdesc,
                           const QString &srcfile, const QString &srcfunc, unsigned long srcline,
                           finException::Level level, finExceptionObject *errobj)
    : _errCode(errcode), _errDesc(errdesc), _srcFile(srcfile),
    _srcPrettyFunction(srcfunc), _srcLine(srcline), _level(level), _errObj(errobj)
{
    this->dump();
}

finException::finException(const finException &e)
    : _errCode(e._errCode), _errDesc(e._errDesc), _srcFile(e._srcFile),
    _srcPrettyFunction(e._srcPrettyFunction), _srcLine(e._srcLine), _errObj(e._errObj)
{ /* Do Nothing */ }

finErrorCode finException::getErrorCode() const
{
    return this->_errCode;
}

const QString &finException::getErrorDescription() const
{
    return this->_errDesc;
}

void finException::raise() const
{
    throw *this;
}

finException *finException::clone() const
{
    return new finException(*this);
}

QString finException::dumpInfo() const
{
    QString retstr = QString();
    QTextStream ts(&retstr);

    ts << _FIN_DEBUGHEAD(this->_srcPrettyFunction, this->_srcFile, this->_srcLine, levelName(this->_level))
       << "[EC:" << this->_errCode << "] " << this->_errDesc;
    if (this->_errObj != nullptr)
        ts << " " << _FIN_DUMPOBJ(this->_errObj);
    return retstr;
}

void finException::dump() const
{
    switch (this->_level) {
    case EL_DEBUG:
        qDebug() << this->dumpInfo();
        break;
    case EL_INFO:
        qInfo() << this->dumpInfo();
        break;
    case EL_WARNING:
        qWarning() << this->dumpInfo();
        break;
    case EL_CRITIAL:
        qCritical() << this->dumpInfo();
        break;
    case EL_FATAL:
        qFatal() << this->dumpInfo();
        break;
    default:
        break;
    }
}

