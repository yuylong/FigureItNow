/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2016 Yulong Yu. All rights reserved.
 */

#include "finErrorCode.h"

const static struct {
    finErrorCode _errCode;
    const QString _errString;
} _finErrorCodeStringMap[] =
{
    // Success
    { finErrorKits::EC_SUCCESS,
        QString("No error")                                      },

    // Warnings
    { finErrorKits::EC_DUPLICATE_OP,
        QString("Duplicated operations")                         },
    { finErrorKits::EC_REACH_BOTTOM,
        QString("Reach accessing bottom")                        },
    { finErrorKits::EC_UNREADY_WARN,
        QString("Unready object detected")                       },
    { finErrorKits::EC_PRECISE_LOSS,
        QString("Success with precise loss")                     },
    { finErrorKits::EC_NORMAL_WARN,
        QString("Success with warning")                          },

    // Errors
    { finErrorKits::EC_NULL_POINTER,
        QString("Access to null pointer")                        },
    { finErrorKits::EC_FILE_NOT_OPEN,
        QString("File does not open")                            },
    { finErrorKits::EC_OUT_OF_MEMORY,
        QString("Out of memory")                                 },
    { finErrorKits::EC_OVERFLOW,
        QString("Calculation overflow")                          },
    { finErrorKits::EC_NOT_FOUND,
        QString("Object not found")                              },
    { finErrorKits::EC_READ_ERROR,
        QString("Reading object failure")                        },
    { finErrorKits::EC_CONTENTION,
        QString("Requested position is occupied.")               },
    { finErrorKits::EC_INVALID_PARAM,
        QString("Invalid Parameter")                             },
    { finErrorKits::EC_STATE_ERROR,
        QString("Operation on a wrong state")                    },
    { finErrorKits::EC_UNKNOWN_ERROR,
        QString("Unkown error")                                  },
    { finErrorKits::EC_NON_IMPLEMENT,
        QString("Non-implement")                                 },

    // Dummy End
    { finErrorKits::EC_DUMMY,
        QString("")                                              }
};

static const QString _finDefaultErrString("Unknown error");

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
    return &_finDefaultErrString;
}

bool
finErrorKits::isErrorResult(enum ErrorCode errcode)
{
    // Any error code less than 0 is recognized as an error.
    return (errcode < 0);
}
