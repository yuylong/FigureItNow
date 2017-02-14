#include "MainWindow.h"
#include "finUiCommandLine.h"
#include <QApplication>

#include <QFile>

#include "finErrorCode.h"
#include "finUiScriptEditor.h"


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

static _guiMain(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    _loadScriptFiles(&w, argc, argv);
    w.show();

    return a.exec();
}

static _consoleMain(int argc, char *argv[])
{
    QApplication a(argc, argv);

    finUiCommandLine cmdline(argc, argv);
    cmdline.work();

    a.quit();
    return 0;
}

int main(int argc, char *argv[])
{
    bool isgui = _isGUIStartUp(argc, argv);

    if ( isgui )
        return _guiMain(argc, argv);
    else
        return _consoleMain(argc, argv);
}
