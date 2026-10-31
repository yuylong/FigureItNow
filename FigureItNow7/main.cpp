/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#include <QApplication>
#include <QFile>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

#include <cstdlib>

#include "MainWindow.h"
#include "finUiCommandLine.h"

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif


#ifdef Q_OS_WIN
/*!
 * \brief Writes every Qt log record as UTF-8 instead of the local 8-bit code page.
 *
 * Qt's default Windows handler falls back to the active ANSI code page (for example GBK) when
 * stderr is a pipe or a file, which garbles translated messages once they are no longer pure
 * ASCII. Writing UTF-8 bytes directly, together with the UTF-8 console code page installed by
 * _setupUtf8Console(), keeps the text readable in consoles, pipes and redirected files alike.
 */
static void _utf8MessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);

    QByteArray line = msg.toUtf8();
    line.append('\n');

    DWORD written = 0;
    HANDLE handle = GetStdHandle(STD_ERROR_HANDLE);
    bool writtenToConsole = false;
    if ( handle != nullptr && handle != INVALID_HANDLE_VALUE ) {
        writtenToConsole = WriteFile(handle, line.constData(),
                                    static_cast<DWORD>(line.size()), &written, nullptr) != FALSE;
    }

    // No console attached (typical for the GUI entry): keep the record visible to a debugger.
    if ( !writtenToConsole ) {
        OutputDebugStringW(msg.toStdWString().c_str());
        OutputDebugStringW(L"\n");
    }

    // Preserve the abort-on-fatal contract of the default handler.
    if ( type == QtFatalMsg )
        abort();
}

/*!
 * \brief Switches the console to UTF-8 and installs the UTF-8 message handler.
 */
static void _setupUtf8Console()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    qInstallMessageHandler(_utf8MessageHandler);
}
#endif


/*!
 * \brief Installs the UI translation matching the system locale.
 *
 * The translation sources live in translations/figureitnow7_<locale>.ts; the build systems compile
 * them into figureitnow7_<locale>.qm and embed the result under ":/i18n". If the system locale has
 * no matching translation, en_US is loaded explicitly so that the UI always runs with a known
 * language instead of relying on the implicit fallback to the source strings. Qt's own strings
 * (standard dialog buttons and the like) are loaded from the Qt translations directory if present.
 * The translators are function-local statics so that they outlive the widgets using them.
 */
static void _installTranslations(QApplication *app)
{
    static QTranslator qtTranslator;
    if ( qtTranslator.load(QLocale(), QStringLiteral("qtbase"), QStringLiteral("_"),
                           QLibraryInfo::path(QLibraryInfo::TranslationsPath)) ) {
        app->installTranslator(&qtTranslator);
    }

    static QTranslator appTranslator;
    if ( !appTranslator.load(QLocale(), QStringLiteral("figureitnow7"), QStringLiteral("_"),
                             QStringLiteral(":/i18n")) ) {
        // Unsupported system language: fall back to English (United States) explicitly.
        // A failed fallback leaves the translator empty, which the check below handles.
        (void)appTranslator.load(QStringLiteral("figureitnow7_en_US"), QStringLiteral(":/i18n"));
    }
    if ( !appTranslator.isEmpty() ) {
        app->installTranslator(&appTranslator);
    }
}

static bool _isGUIStartUp(int argc, char *argv[]) {
    for ( int i = 1; i < argc; i++ ) {
        if ( qstrcmp(argv[i], "-c") == 0 ||
             qstrcmp(argv[i], "--console") == 0 )
            return false;
    }
    return true;
}

static void _loadScriptFiles(MainWindow *mw, int argc, char *argv[])
{
    int filecnt = 0;
    for ( int i = 1; i < argc; i++ ) {
        if ( QFile(QString(argv[i])).exists() ) {
            mw->openScriptFile(QString(argv[i]));
            filecnt++;
        }
    }
    if ( filecnt <= 0 )
        mw->createNewScriptFile();
}

static int _guiMain(int argc, char *argv[])
{
    QApplication a(argc, argv);
    _installTranslations(&a);

    MainWindow w;
    _loadScriptFiles(&w, argc, argv);
    w.show();

    return a.exec();
}

static int _consoleMain(int argc, char *argv[])
{
    QApplication a(argc, argv);
    _installTranslations(&a);

    finUiCommandLine cmdline(argc, argv);
    cmdline.work();

    a.quit();
    return 0;
}

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    _setupUtf8Console();
#endif

    bool isgui = _isGUIStartUp(argc, argv);

    if ( isgui )
        return _guiMain(argc, argv);
    else
        return _consoleMain(argc, argv);
}
