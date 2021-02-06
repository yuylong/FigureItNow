#include "finVersion.h"

#include <QtGlobal>
#include <QObject>
#include <QString>
#include <QTextStream>

finVersionTools::Version finVersionTools::_currentVersion = finVersionTools::version(0, 4, 719);
QString finVersionTools::_currentVersionSuffix = QObject::tr("alpha");

finVersionTools::finVersionTools()
{
    /* Do Nothing. */
}

finVersionTools::Version finVersionTools::version(quint16 major, quint16 minor, quint32 build)
{
    Version verdata;

    verdata.major = major;
    verdata.minor = minor;
    verdata.build = build;

    return verdata;
}

finVersionTools::Version finVersionTools::currentVersion()
{
    return _currentVersion;
}

QString finVersionTools::versionString(quint16 major, quint16 minor, quint32 build,  QString suffix)
{
    Version ver = version(major, minor, build);
    return versionString(ver, suffix);
}

QString finVersionTools::versionString(Version ver, QString suffix)
{
    QString retstr;
    QTextStream(&retstr) << ver.major << "." << ver.minor << "." << ver.build << " " << suffix;
    return retstr;
}

QString finVersionTools::currentVersionString()
{
    return versionString(_currentVersion, _currentVersionSuffix);
}
