#include "includes.hpp"
#include "phantomstyle.h"

#include <QApplication>
#include "version.hpp"

#include "splashscreen.hpp"

void DebugMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg  = msg.toLocal8Bit();
    const char *file     = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
        case QtDebugMsg:
            fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line,
                    function);
            break;
        case QtInfoMsg:
            fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line,
                    function);
            break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line,
                    function);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line,
                    function);
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line,
                    function);
            break;
    }
}

void initConsole()
{
#ifdef _WIN32
    // detach from the current console window
    // if launched from a console window, that will still run waiting for the new console (below) to
    // close it is useful to detach from Qt Creator's <Application output> panel
    FreeConsole();

    // create a separate new console window
    AllocConsole();

    // attach the new console to this application's process
    AttachConsole(GetCurrentProcessId());

    // reopen the std I/O streams to redirect I/O to the new console
    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);
    freopen("CON", "r", stdin);
#endif
}

int main(int argc, char *argv[])
{    
    initConsole();

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);

    SplashScreen splash;
    splash.showMessage("Setting up display format...");
    splash.show();

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setMajorVersion(3);
    format.setMinorVersion(2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QCoreApplication::processEvents();
    splash.showMessage("Configuring style...");

    qInstallMessageHandler(DebugMessageHandler);
    PhantomStyle *style = new PhantomStyle(); // TODO: is this deleted ever???

    QApplication::setStyle(style);

    QPalette pal;
    pal.setColor(QPalette::Window, QColor(0xFF292929));
    pal.setColor(QPalette::Base, QColor(0xFF232323));
    pal.setColor(QPalette::AlternateBase, QColor(0xFF2B2C2D));
    pal.setColor(QPalette::ToolTipBase, QColor(0xFF171819));
    pal.setColor(QPalette::ToolTipText, QColor(0xFFD4D1D0));
    pal.setColor(QPalette::Text, QColor(0xFFD4D1D0));
    pal.setColor(QPalette::WindowText, QColor(0xFFD4D1D0));
    pal.setColor(QPalette::Button, QColor(0xFF333333));
    pal.setColor(QPalette::ButtonText, QColor(0xFFD4D1D0));
    pal.setColor(QPalette::BrightText, QColor(0xFFD4D1D0));

    pal.setColor(QPalette::Highlight, QColor(0xFF545555));
    pal.setColor(QPalette::Light, QColor(0xFF292A2B));
    pal.setColor(QPalette::Midlight, QColor(0xFF333333));
    pal.setColor(QPalette::Mid, QColor(0xFF292929));
    pal.setColor(QPalette::Dark, QColor(0xFF232323));
    pal.setColor(QPalette::HighlightedText, QColor(0xFFD4D1D0));
    pal.setColor(QPalette::Disabled, QPalette::WindowText, QColor(0x60A4A6A8));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(0x60A4A6A8));
    pal.setColor(QPalette::Disabled, QPalette::Text, QColor(0x60A4A6A8));
    pal.setColor(QPalette::Shadow, Qt::black);

    QApplication::setPalette(pal);

    QCoreApplication::processEvents();
    splash.showMessage("Setting up appConfig...");

    appDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/";
    if (!QDir(appDir).exists())
        QDir(appDir).mkpath(appDir);

    if (!QFile(appDir + "appConfig.bin").exists())
        appConfig.write(appDir + "appConfig.bin");
    else
        appConfig.read(appDir + "appConfig.bin");

    homeDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/RetroED/";
    if (!QDir(homeDir).exists())
        QDir(homeDir).mkpath(homeDir);

    splash.showMessage("Setting up file lists...");

    // copy RSDKv4 file list if it doesn't exist
    if (!QFile(homeDir + "RSDKv4FileList.txt").exists()) {
        Reader reader(":/resources/RSDKv4FileList.txt");
        QByteArray bytes = reader.readByteArray(reader.filesize);
        reader.close();

        Writer writer(homeDir + "RSDKv4FileList.txt");
        writer.write(bytes);
        writer.flush();
    }

    // copy RSDKv5 file list if it doesn't exist
    if (!QFile(homeDir + "RSDKv5FileList.txt").exists()) {
        Reader reader(":/resources/RSDKv5FileList.txt");
        QByteArray bytes = reader.readByteArray(reader.filesize);
        reader.close();

        Writer writer(homeDir + "RSDKv5FileList.txt");
        writer.write(bytes);
        writer.flush();
    }

    // copy RSDKv5U file list if it doesn't exist
    if (!QFile(homeDir + "RSDKv5UFileList.txt").exists()) {
        Reader reader(":/resources/RSDKv5UFileList.txt");
        QByteArray bytes = reader.readByteArray(reader.filesize);
        reader.close();

        Writer writer(homeDir + "RSDKv5UFileList.txt");
        writer.write(bytes);
        writer.flush();
    }

    PrintLog("====================================================");
    PrintLog("New RetroED instance: " + QDateTime::currentDateTime().toString());
    PrintLog(QString("Version: ") + RE_VERSION);
    PrintLog("====================================================");

    splash.showMessage("Setting up GameLink...");

    GameLink::Setup();

    MainWindow w;
    splash.finish(&w);
    w.show();
    return a.exec();
}
