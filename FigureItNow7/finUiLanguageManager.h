/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2026 Yulong Yu. All rights reserved.
 */

#ifndef FINUILANGUAGEMANAGER_H
#define FINUILANGUAGEMANAGER_H

#include <QList>
#include <QString>


class QTranslator;

/*!
 * \brief The finUiLanguageManager class owns the application and Qt translators and remembers the user's UI language.
 *
 * The translation sources live in translations/figureitnow7_<locale>.ts. Both build systems compile them into
 * figureitnow7_<locale>.qm and embed the result under ":/i18n", so switching the language does not need any file on
 * disk beside the executable itself. The selected language is stored through QSettings in an INI file below
 * QStandardPaths::AppConfigLocation, under the key "ui/language". A missing (or unparsable) value means
 * finUiLanguageManager::LANG_SYSTEM, i.e. follow the system locale, which is also the default.
 *
 * The manager is a process wide singleton created on first use. It must be created after QApplication has been
 * constructed, because it installs the translators into the application object.
 */
class finUiLanguageManager
{
public:
    /*!
     * \brief The Language enum lists the UI languages offered to the user.
     */
    enum Language {
        LANG_SYSTEM = 0,            //!< Follow the system locale.
        LANG_ENGLISH,               //!< English (United States).
        LANG_SIMPLIFIED_CHINESE,    //!< Chinese (Simplified, China).
        LANG_TRADITIONAL_CHINESE,   //!< Chinese (Traditional, Taiwan).
        LANG_JAPANESE               //!< Japanese (Japan).
    };

    /*!
     * \brief The LanguageInfo struct describes one entry of the language menu.
     */
    struct LanguageInfo {
        Language lang;           //!< The language this entry selects.
        QString code;            //!< The locale code, such as "zh_CN"; empty for LANG_SYSTEM.
        QString nativeName;      //!< The name written in its own language; empty for LANG_SYSTEM.
    };

    static finUiLanguageManager *getInstance();

    static const QList<LanguageInfo> &getAvailableLanguages();
    static QString getLanguageCode(Language lang);
    static Language getLanguageFromCode(const QString &code);

    Language getLanguage() const;
    Language getSavedLanguage() const;

    void applyLanguage(Language lang);
    void applySavedLanguage();

    QString getSettingsFilePath() const;

private:
    explicit finUiLanguageManager();
    ~finUiLanguageManager();

    finUiLanguageManager(const finUiLanguageManager &other) = delete;
    finUiLanguageManager &operator=(const finUiLanguageManager &other) = delete;

    void _installTranslators(Language lang);
    void _saveLanguage(Language lang) const;

    QTranslator *_qtTranslator;
    QTranslator *_appTranslator;
    Language _language;
};

#endif // FINUILANGUAGEMANAGER_H
