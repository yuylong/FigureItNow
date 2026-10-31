/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#include <QApplication>
#include <QFile>

#include <cstdlib>

#include "MainWindow.h"
#include "finUiCommandLine.h"
#include "finUiLanguageManager.h"

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
 * \brief Gives the application the identity used to locate its settings file.
 *
 * QSettings derives the INI path from these names, so they must be set before the language manager reads or writes
 * the settings file.
 */
static void _setupApplicationIdentity()
{
    QCoreApplication::setOrganizationName(QStringLiteral("FigureItNow"));
    QCoreApplication::setApplicationName(QStringLiteral("FigureItNow7"));
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
    _setupApplicationIdentity();
    finUiLanguageManager::getInstance()->applySavedLanguage();

    MainWindow w;
    _loadScriptFiles(&w, argc, argv);
    w.show();

    return a.exec();
}

static int _consoleMain(int argc, char *argv[])
{
    QApplication a(argc, argv);
    _setupApplicationIdentity();
    finUiLanguageManager::getInstance()->applySavedLanguage();

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
