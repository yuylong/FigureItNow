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
    # --always keeps the version non-empty in a work tree without tags, where a plain describe fails.
    APP_VERSION = $$system(git describe --tags --dirty --always)
    isEmpty(GIT_BRANCH): GIT_BRANCH = unknown
    isEmpty(GIT_TIME): GIT_TIME = unknown
    isEmpty(APP_VERSION): APP_VERSION = unknown
} else {
    GIT_BRANCH = unknown
    GIT_TIME = unknown
    APP_VERSION = unknown
}

# The values are compiled in as string literals. shell_quote() is required because GIT_TIME contains
# spaces ("2026-11-14 19:19:25 +0800"): without it qmake splits the value into several words and prefixes
# each of them with -D, which corrupts the compiled-in timestamp.
DEFINES += "GIT_BRANCH=$$shell_quote(\"$$GIT_BRANCH\")"
DEFINES += "GIT_TIME=$$shell_quote(\"$$GIT_TIME\")"
DEFINES += "APP_VERSION=$$shell_quote(\"$$APP_VERSION\")"

TARGET = FigureItNow7
TEMPLATE = app

CONFIG += c++17

# Only MinGW-w64 GCC is supported on Windows (see README_CMAKE.md); MSVC is not. The CMake build
# rejects it at configure time, so the qmake path does the same instead of failing at link time.
win32-msvc*: error("Windows builds require MinGW-w64 GCC; MSVC is not supported. See README_CMAKE.md.")

QMAKE_CXXFLAGS += -Wno-comment

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
    finUiLanguageManager.cpp \
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
    finUiLanguageManager.h \
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

# Multi-language support: translation source files.
# "lrelease" compiles the .ts files into .qm; "embed_translations" puts them into
# the executable's resources under ":/i18n/". Both are only requested when lrelease exists, matching the
# CMake build, where the Linguist tools are an optional component (lrelease ships in its own package on
# several Linux distributions). Without them the build still succeeds and the UI uses the source strings.
TRANSLATIONS += \
    translations/figureitnow7_zh_CN.ts \
    translations/figureitnow7_zh_TW.ts \
    translations/figureitnow7_ja_JP.ts \
    translations/figureitnow7_en_US.ts

win32: LRELEASE_EXE = $$[QT_HOST_BINS]/lrelease.exe
else: LRELEASE_EXE = $$[QT_HOST_BINS]/lrelease

exists($$LRELEASE_EXE) {
    CONFIG += lrelease embed_translations
} else {
    warning("lrelease not found at $$LRELEASE_EXE; building without translations.")
}
