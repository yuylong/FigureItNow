#include "MainWindow.h"
#include <QApplication>

#include <QFile>

#include "finErrorCode.h"


static void _loadScriptFiles(MainWindow *mw, int argc, char *argv[])
{
    int filecnt = 0;
    for ( int i = 1; i < argc; i++ ) {
        //QFile file(QString(argv[i]));
        if ( QFile(QString(argv[i])).exists() ) {
            mw->openScriptFile(QString(argv[i]));
            filecnt++;
        }
    }
    if ( filecnt <= 0 )
        mw->createNewScriptFile();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    _loadScriptFiles(&w, argc, argv);
    w.show();

    return a.exec();
}
