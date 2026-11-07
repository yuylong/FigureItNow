/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2021-2026 Yulong Yu. All rights reserved.
 */

/*! \file finVersion.cpp
 *  \brief Implementations of the finVersionTools metadata and formatting helpers.
 *
 *  Provides the static storage for the Git metadata captured at build time, the normalization that
 *  turns a git describe string into the dotted numeric form installers require, the helper that packs
 *  version components into finVersionTools::Version, and the string-formatting accessors used by the UI
 *  and diagnostic output.
 */

#include "finVersion.h"

#include <QRegularExpression>
#include <QStringList>
#include <QtGlobal>
#include <QObject>
#include <QString>
#include <QTextStream>

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

QString finVersionTools::normalizedVersionString(const QString &description)
{
    static const QRegularExpression numericRe(QStringLiteral("^\\d+(\\.\\d+)*$"));
    static const QRegularExpression digitsRe(QStringLiteral("^\\d+$"));

    QString text = description.trimmed();
    if (text.startsWith(QLatin1Char('v')))
        text.remove(0, 1);
    if (text.endsWith(QLatin1String("-dirty")))
        text.chop(6);

    const QStringList parts = text.split(QLatin1Char('-'), Qt::SkipEmptyParts);
    if (parts.isEmpty())
        return QStringLiteral("0.0.0");

    const QString base = parts.first();
    if (!numericRe.match(base).hasMatch())
        return QStringLiteral("0.0.0");

    /* The last all-digit segment is the commit count since the last tag. */
    QString revision;
    for (const QString &part : parts) {
        if (digitsRe.match(part).hasMatch())
            revision = part;
    }

    if (revision.isEmpty())
        return base;
    return base + QLatin1Char('.') + revision;
}

QString finVersionTools::currentVersionString()
{
    return normalizedVersionString(_currentGitVersion);
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
