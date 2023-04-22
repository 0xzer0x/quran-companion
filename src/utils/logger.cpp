#include "logger.h"

QString Logger::filename = "qc-logs.txt";
static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(nullptr);
QFile Logger::logFile = QFile(filename);

Logger::Logger(QObject *parent)
    : QObject{parent}
{}

bool Logger::startLogger(QString parentDir)
{
    logFile.setFileName(parentDir + QDir::separator() + filename);
    return logFile.open(QIODevice::WriteOnly);
}

void Logger::stopLogger()
{
    logFile.close();
    qInstallMessageHandler(nullptr);
}

void Logger::attach()
{
    qInstallMessageHandler(Logger::handler);
}

void Logger::handler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("[DEBUG] %0 ").arg(msg);
        break;

    case QtWarningMsg:
        txt = QString("[WARNING] %0 ").arg(msg);
        break;

    case QtCriticalMsg:
        txt = QString("[CRITICAL] %0 ").arg(msg);
        break;

    case QtFatalMsg:
        txt = QString("[FATAL] %0 ").arg(msg);
        break;

    case QtInfoMsg:
        txt = QString("[INFO] %0 ").arg(msg);
        break;
    }

    QTextStream ts(&Logger::logFile);

    ts << '[' << QDateTime::currentDateTime().toString() << ']' << txt << " file: " << context.file
       << " line: " << context.line << "\n";

    ts.flush();
}
