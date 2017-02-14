#include "finUiCommandLine.h"

#include <QDebug>
#include <QFile>
#include <QPdfWriter>
#include <QSvgGenerator>

#include "finFigureContainer.h"
#include "finExecMachine.h"
#include "finExecEnvironment.h"
#include "finGraphPanelWidget.h"


finUiCommandLine::finUiCommandLine()
    : _inFileList()
{
    this->_outType = QString("PDF");
}

finUiCommandLine::finUiCommandLine(int argc, char *argv[])
    : _inFileList()
{
    this->parseArgument(argc, argv);
}

finUiCommandLine::finUiCommandLine(const QStringList &arglist)
    : _inFileList()
{
    this->parseArgument(arglist);
}

finErrorCode finUiCommandLine::reset()
{
    this->_inFileList.clear();
    this->_outType = QString("PDF");
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

QStringList finUiCommandLine::parseStringList(int argc, char *argv[])
{
    QStringList retlist;
    for ( int i = 0; i < argc; i++ ) {
        retlist.append(QString(argv[i]));
    }
    return retlist;
}

QString finUiCommandLine::parseArgumentCommand(const QString &argstr)
{
    if ( QString::compare(argstr, QString("-i")) == 0 ||
         QString::compare(argstr, QString("--input")) == 0 ) {
        return QString("input");
    } else if ( QString::compare(argstr, QString("-t")) == 0 ||
                QString::compare(argstr, QString("--output-type")) == 0 ) {
        return QString("outtype");
    } else if ( QString::compare(argstr, QString("-c")) == 0 ||
                QString::compare(argstr, QString("--console")) == 0 ) {
        return QString("ignore");
    } else {
        return QString();
    }
}

finErrorCode finUiCommandLine::parseArgument(int argc, char *argv[])
{
    QStringList arglist = this->parseStringList(argc, argv);
    return this->parseArgument(arglist);
}

finErrorCode finUiCommandLine::parseArgument(const QStringList &arglist)
{
    QStringList inarglist = arglist;
    inarglist.removeFirst();

    QString cmd = QString();
    QString curarg;
    int cmdargidx = 0;
    foreach ( curarg, inarglist ) {
        if ( cmd.isEmpty() ) {
            cmd = this->parseArgumentCommand(curarg);
            if ( cmd.isEmpty() ) {
                this->_inFileList.append(curarg);
            } else if ( QString::compare(cmd, QString("ignore")) == 0 ) {
                cmd = QString();
            } else {
                cmdargidx = 0;
                continue;
            }
        } else if ( QString::compare(cmd, QString("input")) == 0 ) {
            if ( cmdargidx == 0 )
                this->_inFileList.append(curarg);
            cmd = QString();
        } else if ( QString::compare(cmd, QString("outtype")) == 0 ) {
            if ( cmdargidx == 0 )
                this->_outType = curarg;
            cmd = QString();
        }
        cmdargidx++;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finUiCommandLine::work()
{
    qInfo() << "Input file count: " << this->_inFileList.count();
    qInfo() << "Output type: " << this->_outType;

    if ( this->_inFileList.count() <= 0 ) {
        qWarning() << "No file to handle!";
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;
    }

    int success = 0;
    if ( QString::compare(this->_outType, QString("PDF"), Qt::CaseInsensitive) == 0 ) {
        success = this->figureToPDF();
    } else if ( QString::compare(this->_outType, QString("SVG"), Qt::CaseInsensitive) == 0 ) {
        success = this->figureToSVG();
    } else {
        qWarning() << "The output type is not supported!";
        return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
    }

    qInfo() << "Successful Count: " << success;
    if ( success <= 0 )
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;
    else
        return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finUiCommandLine::compileAndRunScript(const QString &filename, finFigureContainer *outfig)
{
    QFile infile(filename);
    bool openok = infile.open(QIODevice::ReadOnly);
    if ( !openok ) {
        qWarning() << "Cannot open file: " << filename << endl;
        return finErrorCodeKits::FIN_EC_FILE_NOT_OPEN;
    }

    QString scriptcode = infile.readAll();
    infile.close();

    finErrorCode errcode;
    finExecMachine machine;
    machine.initEnvironmentFromRoot();
    machine.setFigureContainer(outfig);
    machine.setScriptCode(scriptcode);

    errcode = machine.compile();
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        qWarning() << "Compile script failed: " << filename;
        return errcode;
    }

    errcode = machine.execute();
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        qWarning() << "Execute script failed: " << filename;
        return errcode;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

int finUiCommandLine::figureToPDF()
{
    int succ = 0;
    QString filename;
    foreach ( filename, this->_inFileList ) {
        QString outfilename = filename + QString(".pdf");
        finFigureContainer figcontainer;

        finErrorCode errcode = this->compileAndRunScript(filename, &figcontainer);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            continue;

        static const int _defResolution = 72;

        finGraphConfig *graphcfg = figcontainer.getGraphConfig();
        QPdfWriter pdfwrt(outfilename);

        pdfwrt.setTitle(filename);
        pdfwrt.setCreator(QString("FigureItNow 7"));
        pdfwrt.setPageSize(QPageSize(graphcfg->getPanelPixelSize() / _defResolution, QPageSize::Inch));
        pdfwrt.setPageMargins(QMarginsF(0.0, 0.0, 0.0, 0.0));
        pdfwrt.setResolution(_defResolution);

        finGraphPanelWidget graphpanel;
        graphpanel.setWidget(&pdfwrt);
        graphpanel.setFigureContainer(&figcontainer);

        errcode = graphpanel.draw();
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            qWarning() << "Draw on panel failed: " << filename;
            continue;
        }

        qInfo() << "Figour OK: " << outfilename;
        succ++;
    }
    return succ;
}

int finUiCommandLine::figureToSVG()
{
    int succ = 0;
    QString filename;
    foreach ( filename, this->_inFileList ) {
        QString outfilename = filename + QString(".svg");
        finFigureContainer figcontainer;

        finErrorCode errcode = this->compileAndRunScript(filename, &figcontainer);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            continue;

        finGraphConfig *graphcfg = figcontainer.getGraphConfig();
        QSvgGenerator svggen;
        svggen.setFileName(outfilename);
        svggen.setTitle(filename);
        svggen.setDescription(QString("Generated by FigureItNow7."));
        svggen.setSize(graphcfg->getPanelPixelSize().toSize());
        svggen.setViewBox(graphcfg->getWholePanelPixelRect().toRect());

        finGraphPanelWidget graphpanel;
        graphpanel.setWidget(&svggen);
        graphpanel.setFigureContainer(&figcontainer);

        errcode = graphpanel.draw();
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            qWarning() << "Draw on panel failed: " << filename;
            continue;
        }

        qInfo() << "Figour OK: " << outfilename;
        succ++;
    }
    return succ;
}
