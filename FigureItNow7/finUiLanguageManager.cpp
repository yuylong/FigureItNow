/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2026 Yulong Yu. All rights reserved.
 */

#include <QCoreApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QSettings>
#include <QTranslator>

#include "finUiLanguageManager.h"


static const char *_SETTINGS_LANGUAGE_KEY = "ui/language";
static const char *_TRANS_RESOURCE_DIR = ":/i18n";
static const char *_TRANS_FILE_PREFIX = "figureitnow7";
static const char *_TRANS_FALLBACK_FILE = "figureitnow7_en_US";


/*!
 * \brief Builds a QSettings object pointing to the application INI file.
 *
 * The path is QStandardPaths::AppConfigLocation + "/FigureItNow7.ini", for example
 * "%APPDATA%\\FigureItNow\\FigureItNow7.ini" on Windows and "~/.config/FigureItNow/FigureItNow7.ini" on Linux.
 * QSettings is neither copyable nor movable, so the caller owns the returned object.
 */
static QSettings *_createSettings()
{
    return new QSettings(QSettings::IniFormat, QSettings::UserScope,
                         QCoreApplication::organizationName(), QCoreApplication::applicationName());
}

finUiLanguageManager::finUiLanguageManager()
{
    this->_qtTranslator = nullptr;
    this->_appTranslator = nullptr;
    this->_language = LANG_SYSTEM;
}

finUiLanguageManager::~finUiLanguageManager()
{
    QCoreApplication *app = QCoreApplication::instance();
    if ( app != nullptr ) {
        if ( this->_appTranslator != nullptr )
            app->removeTranslator(this->_appTranslator);
        if ( this->_qtTranslator != nullptr )
            app->removeTranslator(this->_qtTranslator);
    }

    delete this->_appTranslator;
    delete this->_qtTranslator;
}

finUiLanguageManager *finUiLanguageManager::getInstance()
{
    static finUiLanguageManager _instance;
    return &_instance;
}

const QList<finUiLanguageManager::LanguageInfo> &finUiLanguageManager::getAvailableLanguages()
{
    static const QList<LanguageInfo> _langlist = {
        { LANG_SYSTEM,              QString(),                      QString() },
        { LANG_ENGLISH,             QStringLiteral("en_US"),       QStringLiteral("English") },
        { LANG_SIMPLIFIED_CHINESE,  QStringLiteral("zh_CN"),       QStringLiteral("简体中文") },
        { LANG_TRADITIONAL_CHINESE, QStringLiteral("zh_TW"),       QStringLiteral("繁體中文") },
        { LANG_JAPANESE,            QStringLiteral("ja_JP"),       QStringLiteral("日本語") }
    };
    return _langlist;
}

QString finUiLanguageManager::getLanguageCode(Language lang)
{
    const QList<LanguageInfo> &langlist = getAvailableLanguages();
    for ( int i = 0; i < langlist.count(); i++ ) {
        if ( langlist.at(i).lang == lang )
            return langlist.at(i).code;
    }
    return QString();
}

finUiLanguageManager::Language finUiLanguageManager::getLanguageFromCode(const QString &code)
{
    if ( code.isEmpty() )
        return LANG_SYSTEM;

    const QList<LanguageInfo> &langlist = getAvailableLanguages();
    for ( int i = 0; i < langlist.count(); i++ ) {
        if ( QString::compare(langlist.at(i).code, code, Qt::CaseInsensitive) == 0 )
            return langlist.at(i).lang;
    }
    return LANG_SYSTEM;
}

finUiLanguageManager::Language finUiLanguageManager::getLanguage() const
{
    return this->_language;
}

finUiLanguageManager::Language finUiLanguageManager::getSavedLanguage() const
{
    QSettings *settings = _createSettings();
    QString code = settings->value(QString::fromLatin1(_SETTINGS_LANGUAGE_KEY)).toString();
    delete settings;

    return getLanguageFromCode(code);
}

void finUiLanguageManager::applyLanguage(Language lang)
{
    if ( lang < LANG_SYSTEM || lang > LANG_JAPANESE )
        lang = LANG_SYSTEM;

    this->_installTranslators(lang);
    this->_language = lang;
    this->_saveLanguage(lang);
}

void finUiLanguageManager::applySavedLanguage()
{
    Language lang = this->getSavedLanguage();
    this->_installTranslators(lang);
    this->_language = lang;
}

QString finUiLanguageManager::getSettingsFilePath() const
{
    QSettings *settings = _createSettings();
    QString path = settings->fileName();
    delete settings;

    return path;
}

/*!
 * \brief Replaces the installed translators with the ones matching the given language.
 *
 * Both QCoreApplication::removeTranslator() and QCoreApplication::installTranslator() post a
 * QEvent::LanguageChange to every existing widget, which is where the widgets retranslate themselves. During the
 * start up no widget exists yet, so the first call just makes the translations available.
 */
void finUiLanguageManager::_installTranslators(Language lang)
{
    QCoreApplication *app = QCoreApplication::instance();
    if ( app == nullptr )
        return;

    if ( this->_qtTranslator == nullptr )
        this->_qtTranslator = new QTranslator();
    if ( this->_appTranslator == nullptr )
        this->_appTranslator = new QTranslator();

    app->removeTranslator(this->_appTranslator);
    app->removeTranslator(this->_qtTranslator);

    QLocale locale = (lang == LANG_SYSTEM) ? QLocale() : QLocale(getLanguageCode(lang));

    // Qt's own strings (standard dialog buttons and the like) come from the Qt translations directory. A missing file
    // is tolerated: the standard buttons then simply stay in English.
    if ( this->_qtTranslator->load(locale, QStringLiteral("qtbase"), QStringLiteral("_"),
                                   QLibraryInfo::path(QLibraryInfo::TranslationsPath)) ) {
        app->installTranslator(this->_qtTranslator);
    }

    bool loaded = false;
    if ( lang == LANG_SYSTEM )
        loaded = this->_appTranslator->load(locale, QString::fromLatin1(_TRANS_FILE_PREFIX), QStringLiteral("_"),
                                            QString::fromLatin1(_TRANS_RESOURCE_DIR));
    else
        loaded = this->_appTranslator->load(QString::fromLatin1(_TRANS_FILE_PREFIX) + QStringLiteral("_") +
                                            getLanguageCode(lang),
                                            QString::fromLatin1(_TRANS_RESOURCE_DIR));

    if ( !loaded ) {
        // Unsupported locale: fall back to English (United States) explicitly so that the UI always runs with a known
        // language instead of relying on the implicit fallback to the source strings.
        loaded = this->_appTranslator->load(QString::fromLatin1(_TRANS_FALLBACK_FILE),
                                            QString::fromLatin1(_TRANS_RESOURCE_DIR));
    }
    if ( loaded )
        app->installTranslator(this->_appTranslator);
}

/*!
 * \brief Writes the selected language into the settings file, so that the next start uses it again.
 *
 * LANG_SYSTEM is stored as "no value" rather than as an explicit "system" marker: the application then follows
 * the system locale, including later changes of it.
 */
void finUiLanguageManager::_saveLanguage(Language lang) const
{
    QSettings *settings = _createSettings();
    QString code = getLanguageCode(lang);
    if ( code.isEmpty() )
        settings->remove(QString::fromLatin1(_SETTINGS_LANGUAGE_KEY));
    else
        settings->setValue(QString::fromLatin1(_SETTINGS_LANGUAGE_KEY), code);
    settings->sync();
    delete settings;
}
