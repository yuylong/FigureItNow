/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2021-2026 Yulong Yu. All rights reserved.
 */

/*! \file finVersion.h
 *  \brief Declarations of version-formatting and build-metadata helpers.
 *
 *  This header defines finVersionTools, a namespace-like utility class that stores the Git metadata
 *  captured at build time, derives an installer-friendly numeric version from it, and exposes helpers
 *  for formatting version components as strings.
 */

#ifndef FINVERSION_H
#define FINVERSION_H

#include <QtGlobal>
#include <QObject>
#include <QString>

/*! \class finVersionTools
 *  \brief Global helpers for application version and Git build metadata.
 *
 *  finVersionTools is used as a namespace. It owns the Git metadata compiled into the program,
 *  normalizes the git describe output into the dotted numeric form required by installers, and exposes
 *  the Git branch, build date, and descriptive version string injected by CMake.
 *
 *  The normalization rule is duplicated in packaging/build_installer.ps1 so that the About dialog and
 *  the generated installer report the same version. Both implementations must stay in sync: strip a
 *  leading "v" and a trailing "-dirty", take the first dash-separated segment as the base version, and
 *  append the last all-digit segment as the revision. For example, "v0.5-alpha-172-g1ed173e-dirty"
 *  normalizes to "0.5.172".
 */
class finVersionTools
{
public:
    /*! \union finVersionTools::Version
     *  \brief Packed semantic-version value used by finVersionTools.
     *
     *  The union provides both a raw 64-bit representation and named fields for the major, minor,
     *  and revision components so callers can pass versions around as a single value while still
     *  formatting individual parts when needed.
     */
    union Version {
        quint64 rawdata;  //!< Raw packed 64-bit representation of the version.
        struct {
            quint16 revision;  //!< Revision / patch component.
            quint16 minor;     //!< Minor component.
            quint32 major;     //!< Major component.
        };
    };

private:
    static QString _currentGitBranch;       //!< Git branch name captured at build time.
    static QString _currentGitDate;         //!< Git commit date string captured at build time.
    static QString _currentGitVersion;      //!< Git describe / app version string captured at build time.

    /*!
     *  \brief Default constructor (unused).
     *
     *  finVersionTools is a namespace-like utility class and is not intended to be instantiated.
     */
    finVersionTools();

public:
    /*!
     *  \brief Packs version components into a Version value.
     *
     *  \param major     Major version component.
     *  \param minor     Minor version component.
     *  \param revision  Revision / patch component.
     *  \return A Version union whose named fields and raw packed value represent the supplied components.
     */
    static Version version(quint16 major, quint16 minor = 0, quint32 revision = 0);

    /*!
     *  \brief Formats explicit version components as a string.
     *
     *  This overload first packs the supplied components into a Version value, then delegates to the
     *  Version-based formatter.
     *
     *  \param major     Major version component.
     *  \param minor     Minor version component.
     *  \param revision  Revision / patch component.
     *  \param prefix    String prepended before the numeric version.
     *  \param suffix    String appended after a trailing hyphen.
     *  \return Formatted version string.
     */
    static QString versionString(quint16 major, quint16 minor = 0, quint32 revision = 0,
                                 QString prefix = "v", QString suffix = QString());

    /*!
     *  \brief Formats a packed Version value as a string.
     *
     *  The output format is prefix + major.minor, optionally followed by .revision when the revision is
     *  non-zero, and then a trailing hyphen plus suffix.
     *
     *  \param ver     Packed version value to render.
     *  \param prefix  String prepended before the numeric version.
     *  \param suffix  String appended after a trailing hyphen.
     *  \return Formatted version string.
     */
    static QString versionString(Version ver, QString prefix = "v", QString suffix = QString());

    /*!
     *  \brief Converts a git describe string into a dotted numeric version.
     *
     *  The result only contains digits and dots, which is what installers such as Qt Installer Framework
     *  require. Strings that cannot be parsed fall back to "0.0.0".
     *
     *  \param description  Output of "git describe --tags --dirty".
     *  \return Dotted numeric version, for example "0.5.172".
     */
    static QString normalizedVersionString(const QString &description);

    /*!
     *  \brief Returns the dotted numeric version derived from the compiled-in git describe string.
     */
    static QString currentVersionString();

    /*!
     *  \brief Returns the Git branch name captured at build time.
     */
    static const QString &currentGitBranch();

    /*!
     *  \brief Returns the Git date string captured at build time.
     */
    static const QString &currentGitDate();

    /*!
     *  \brief Returns the descriptive Git version string captured at build time.
     */
    static const QString &currentGitVersion();
};

#endif // FINVERSION_H
