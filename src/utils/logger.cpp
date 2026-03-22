#include "logger.h"
#include <Qt>
#include <QtLogging>

static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(nullptr);
QString Logger::filename = "qc.log";
QFile Logger::logFile = QFile(filename);

Logger::Logger(QObject* parent)
  : QObject{ parent }
{
}

bool
Logger::startLogger(QString parentDir)
{
  logFile.setFileName(parentDir + QDir::separator() + filename);
  return logFile.open(QIODevice::Append);
}

void
Logger::stopLogger()
{
  logFile.close();
  qInstallMessageHandler(nullptr);
}

void
Logger::attach()
{
  qSetMessagePattern("[%{time}%{if-debug}, %{file}:%{function}:%{line}%{endif}, %{type}] %{message}");
  qInstallMessageHandler(Logger::handler);
}

void
Logger::handler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
  // NOTE: Send logs to default handler (stdout)
  if (QT_DEFAULT_MESSAGE_HANDLER) {
    (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);
  }

  // NOTE: Additonally send the logs to log file
  QDateTime now = QDateTime::currentDateTimeUtc();
  QString timestamp = now.toString(Qt::ISODate);
  QString format = "[%0, %1] %2\n";
  QString level = "info";
  switch (type) {
    case QtDebugMsg:
      level = "debug";
      break;
    case QtInfoMsg:
      level = "info";
      break;
    case QtWarningMsg:
      level = "warning";
      break;
    case QtCriticalMsg:
      level = "critical";
      break;
    case QtFatalMsg:
      level = "fatal";
      break;
  }

  QString txt = format.arg(timestamp, level, msg);
  QTextStream ts(&Logger::logFile);
  ts << txt;
  ts.flush();
}
