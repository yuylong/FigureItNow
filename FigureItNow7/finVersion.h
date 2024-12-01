/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2021-2024 Yulong Yu. All rights reserved.
 */

#ifndef FINVERSION_H
#define FINVERSION_H

#include <QtGlobal>
#include <QObject>
#include <QString>

class finVersionTools
{
public:
    union Version {
        quint64 rawdata;
        struct {
            quint16 revision;
            quint16 minor;
            quint32 major;
        };
    };

private:
    static Version _currentVersion;
    static QString _currentVersionPrefix;
    static QString _currentVersionSuffix;

    static QString _currentGitBranch;
    static QString _currentGitDate;
    static QString _currentGitVersion;

    finVersionTools();

public:
    static Version version(quint16 major, quint16 minor = 0, quint32 revision = 0);
    static Version currentVersion();

    static QString versionString(quint16 major, quint16 minor = 0, quint32 revision = 0,
                                 QString prefix = "v", QString suffix = QString());
    static QString versionString(Version ver, QString prefix = "v", QString suffix = QString());
    static QString currentVersionString();

    static const QString &currentGitBranch();
    static const QString &currentGitDate();
    static const QString &currentGitVersion();
};

#endif // FINVERSION_H
