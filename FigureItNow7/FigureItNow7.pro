#-------------------------------------------------
#
# Project created by QtCreator 2016-01-17T12:55:25
#
#-------------------------------------------------

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets svg printsupport

system ("git status") {
    GIT_BRANCH = $$system(git rev-parse --abbrev-ref HEAD)
    GIT_TIME = $$system(git show --oneline --format=\"%ci\" -s HEAD)
    APP_VERSION = $$system(git describe --tags --dirty)
} else {
    GIT_BRANCH = None
    GIT_TIME = None
    APP_VERSION = None
}

DEFINES += GIT_BRANCH=\"\\\"$$GIT_BRANCH\\\"\"
DEFINES += GIT_TIME=\"\\\"$$GIT_TIME\\\"\"
DEFINES += APP_VERSION=\"\\\"$$APP_VERSION\\\"\"

TARGET = FigureItNow7
TEMPLATE = app

SOURCES += main.cpp\
        MainWindow.cpp \
    finErrorCode.cpp \
    finFigureObject.cpp \
    finLexNode.cpp \
    finLexReader.cpp \
    finSyntaxErrorDump.cpp \
    finSyntaxErrorList.cpp \
    finSyntaxNode.cpp \
    finSyntaxReader.cpp \
    finSyntaxOptimzer.cpp \
    finSyntaxTree.cpp \
    finSyntaxError.cpp \
    finExecVariable.cpp \
    finExecFunction.cpp \
    finExecEnvironment.cpp \
    finGraphPanelBase.cpp \
    finGraphConfig.cpp \
    finFigureContainer.cpp \
    finExecOperartorCalc.cpp \
    finExecMachine.cpp \
    finFigureConfig.cpp \
    finExecFlowControl.cpp \
    finExecCompiler.cpp \
    finExecFuncMatrix.cpp \
    finExecFuncFiguring.cpp \
    finExecFuncMath.cpp \
    finExecFuncString.cpp \
    finExecFuncSystem.cpp \
    finExecFuncFile.cpp \
    finGraphTrans.cpp \
    finGraphPanelPainter.cpp \
    finGraphPanelWidget.cpp \
    finFigurePoint3D.cpp \
    finUiEdtrFontConfigDlg.cpp \
    finUiFigureWidget.cpp \
    finGraphPanelScene.cpp \
    finFigureAlg.cpp \
    finFigurePath.cpp \
    finFigureArrow.cpp \
    finUiFigConfigDlg.cpp \
    finUiColorLabel.cpp \
    finUiGraphConfigDlg.cpp \
    finPlotFunction.cpp \
    finPlotDots.cpp \
    finExecFuncPlot.cpp \
    finPlotPolar.cpp \
    finPlotEquation2D.cpp \
    finPlotParametric.cpp \
    finUiSyntaxHighlighter.cpp \
    finUiScriptEditor.cpp \
    finUiAboutDlg.cpp \
    finUiCommandLine.cpp \
    finExecAlg.cpp \
    finExecVariableSysvar.cpp \
    finUiSysFuncList.cpp \
    finVersion.cpp

HEADERS  += MainWindow.h \
    finErrorCode.h \
    finFigureObject.h \
    finLexNode.h \
    finLexReader.h \
    finSyntaxErrorDump.h \
    finSyntaxErrorList.h \
    finSyntaxNode.h \
    finSyntaxReader.h \
    finSyntaxOptimzer.h \
    finSyntaxTree.h \
    finSyntaxError.h \
    finExecVariable.h \
    finExecFunction.h \
    finExecEnvironment.h \
    finGraphPanelBase.h \
    finGraphConfig.h \
    finFigureContainer.h \
    finExecOperartorCalc.h \
    finExecMachine.h \
    finFigureConfig.h \
    finExecFlowControl.h \
    finExecCompiler.h \
    finGraphTrans.h \
    finGraphPanelPainter.h \
    finGraphPanelWidget.h \
    finFigurePoint3D.h \
    finUiEdtrFontConfigDlg.h \
    finUiFigureWidget.h \
    finGraphPanelScene.h \
    finFigureAlg.h \
    finFigurePath.h \
    finFigureArrow.h \
    finUiFigConfigDlg.h \
    finUiColorLabel.h \
    finUiGraphConfigDlg.h \
    finPlotFunction.h \
    finPlotDots.h \
    finPlotPolar.h \
    finPlotEquation2D.h \
    finPlotParametric.h \
    finUiSyntaxHighlighter.h \
    finUiScriptEditor.h \
    finUiAboutDlg.h \
    finUiCommandLine.h \
    finExecAlg.h \
    finUiSysFuncList.h \
    finVersion.h

FORMS    += MainWindow.ui \
    finUiEdtrFontConfigDlg.ui \
    finUiFigConfigDlg.ui \
    finUiGraphConfigDlg.ui \
    finUiScriptEditor.ui \
    finUiAboutDlg.ui \
    finUiSysFuncList.ui

DISTFILES +=

RESOURCES += \
    Icons.qrc
