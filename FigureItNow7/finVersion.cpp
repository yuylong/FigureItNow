/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2021-2024 Yulong Yu. All rights reserved.
 */

/*! \file finVersion.cpp
 *  \brief Implementations of the finVersionTools metadata and formatting helpers.
 *
 *  Provides the static storage for the application's compiled-in version and Git metadata, the helper
 *  that packs version components into finVersionTools::Version, and the string-formatting accessors used
 *  by the UI and diagnostic output.
 */

#include "finVersion.h"

#include <QtGlobal>
#include <QObject>
#include <QString>
#include <QTextStream>

finVersionTools::Version finVersionTools::_currentVersion = finVersionTools::version(0, 5, 0);
QString finVersionTools::_currentVersionPrefix = QString("v");
QString finVersionTools::_currentVersionSuffix = QString("alpha");
QString finVersionTools::_currentGitBranch = QString(GIT_BRANCH);
QString finVersionTools::_currentGitDate = QString(GIT_TIME).replace("-D", "");
QString finVersionTools::_currentGitVersion = QString(APP_VERSION);

finVersionTools::finVersionTools()
{
    /* Do Nothing. */
}

finVersionTools::Version finVersionTools::version(quint16 major, quint16 minor, quint32 revision)
{
    Version verdata;

    verdata.major = major;
    verdata.minor = minor;
    verdata.revision = revision;

    return verdata;
}

finVersionTools::Version finVersionTools::currentVersion()
{
    return _currentVersion;
}

QString finVersionTools::versionString(quint16 major, quint16 minor, quint32 build,  QString prefix, QString suffix)
{
    Version ver = version(major, minor, build);
    return versionString(ver, prefix, suffix);
}

QString finVersionTools::versionString(Version ver, QString prefix, QString suffix)
{
    QString retstr;
    QTextStream retstrts = QTextStream(&retstr);

    retstrts << prefix << ver.major << "." << ver.minor;
    if (ver.revision != 0)
        retstrts << "." << ver.revision;
    retstrts << "-" << suffix;
    return retstr;
}

QString finVersionTools::currentVersionString()
{
    return versionString(_currentVersion, _currentVersionPrefix, _currentVersionSuffix);
}

const QString &finVersionTools::currentGitBranch()
{
    return _currentGitBranch;
}

const QString &finVersionTools::currentGitDate()
{
    return _currentGitDate;
}

const QString &finVersionTools::currentGitVersion()
{
    return _currentGitVersion;
}
