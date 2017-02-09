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
    { finErrorCodeKits::FIN_EC_SUCCESS,
        QString("No error")                                      },

    // Warnings
    { finErrorCodeKits::FIN_EC_DUPLICATE_OP,
        QString("Duplicated operations")                         },
    { finErrorCodeKits::FIN_EC_REACH_BOTTOM,
        QString("Reach accessing bottom")                        },
    { finErrorCodeKits::FIN_EC_UNREADY_WARN,
        QString("Unready object detected")                       },
    { finErrorCodeKits::FIN_EC_PRECISE_LOSS,
        QString("Success with precise loss")                     },
    { finErrorCodeKits::FIN_EC_NORMAL_WARN,
        QString("Success with warning")                          },

    // Errors
    { finErrorCodeKits::FIN_EC_NULL_POINTER,
        QString("Access to null pointer")                        },
    { finErrorCodeKits::FIN_EC_FILE_NOT_OPEN,
        QString("File does not open")                            },
    { finErrorCodeKits::FIN_EC_OUT_OF_MEMORY,
        QString("Out of memory")                                 },
    { finErrorCodeKits::FIN_EC_OVERFLOW,
        QString("Calculation overflow")                          },
    { finErrorCodeKits::FIN_EC_NOT_FOUND,
        QString("Object not found")                              },
    { finErrorCodeKits::FIN_EC_READ_ERROR,
        QString("Reading object failure")                        },
    { finErrorCodeKits::FIN_EC_CONTENTION,
        QString("Requested position is occupied.")               },
    { finErrorCodeKits::FIN_EC_INVALID_PARAM,
        QString("Invalid Parameter")                             },
    { finErrorCodeKits::FIN_EC_STATE_ERROR,
        QString("Operation on a wrong state")                    },
    { finErrorCodeKits::FIN_EC_UNKNOWN_ERROR,
        QString("Unkown error")                                  },
    { finErrorCodeKits::FIN_EC_NON_IMPLEMENT,
        QString("Non-implement")                                 },

    // Dummy End
    { finErrorCodeKits::FIN_EC_DUMMY,
        QString("")                                              }
};

static const QString _finDefaultErrString("Unknown error");

finErrorCodeKits::finErrorCodeKits()
{
    // We keep this function public, not private or protected, allowing the upper layered code create the instance of
    // this class. But it is not reaaly necessary, because currently all the functions in this class is static.
    return;
}

const QString *
finErrorCodeKits::errcodeToString(enum finErrorCodeType errcode)
{
    int i = 0;

    // Iterate all the error codes, and return the corresponding error description string defined in
    // _finErrorCodeStringMap.
    while (true) {
        if (_finErrorCodeStringMap[i]._errCode == finErrorCodeKits::FIN_EC_DUMMY)
            break;

        if (_finErrorCodeStringMap[i]._errCode == errcode)
            return &_finErrorCodeStringMap[i]._errString;
        i++;
    }
    return &_finDefaultErrString;
}

bool
finErrorCodeKits::isErrorResult(enum finErrorCodeType errcode)
{
    // Any error code less than 0 is recognized as an error.
    return (errcode < 0);
}
