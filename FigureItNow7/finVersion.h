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
            quint32 build;
            quint16 minor;
            quint16 major;
        };
    };

private:
    static Version _currentVersion;
    static QString _currentVersionSuffix;

    finVersionTools();

public:
    static Version version(quint16 major, quint16 minor = 0, quint32 build = 0);
    static Version currentVersion();

    static QString versionString(quint16 major, quint16 minor = 0, quint32 build = 0,
                                 QString suffix = QString());
    static QString versionString(Version ver, QString suffix = QString());
    static QString currentVersionString();
};

#endif // FINVERSION_H
