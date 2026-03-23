#include "logger.h"
#include <QFile>
#include <QFileInfo>
#include <Qt>
#include <QtLogging>

static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(nullptr);
QString Logger::filename = "qc.log";
QFile Logger::logFile = QFile(filename);
int Logger::maxFileAge = 30;
int Logger::maxFileSize = 50;

Logger::Logger(QObject* parent)
  : QObject{ parent }
{
}

bool
Logger::startLogger(QString parentDir)
{
  // NOTE: Set default handler format to match custom handler for consistency
  qSetMessagePattern("[%{time}%{if-debug}, %{file}:%{function}:%{line}%{endif}, %{type}] %{message}");

  // NOTE: Set path for the logs file
  logFile.setFileName(parentDir + QDir::separator() + filename);

  // NOTE: Check logfile truncation conditions:
  // - File older than Logger::maxFileAge days
  // - File size is more than Logger::maxFileSize MiB
  bool tooOld = false, tooLarge = false;
  QFileInfo fileInfo(logFile);
  QDateTime now = QDateTime::currentDateTime();
  if (fileInfo.exists()) {
    tooOld = fileInfo.birthTime() < now.addDays(-maxFileAge);
    tooLarge = fileInfo.size() > maxFileSize * 1024 * 1024;
  }
  if (tooOld || tooLarge) {
    QString condition =
      tooOld ? QString("age more than %0 days").arg(maxFileAge) : QString("size more than %0 MiB").arg(maxFileSize);
    qInfo("Logs file %s, recreating log file", qPrintable(condition));
    logFile.remove();
  }

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
  QString level = "INFO";
  switch (type) {
    case QtDebugMsg:
      level = "DEBUG";
      break;
    case QtInfoMsg:
      level = "INFO";
      break;
    case QtWarningMsg:
      level = "WARNING";
      break;
    case QtCriticalMsg:
      level = "CRITICAL";
      break;
    case QtFatalMsg:
      level = "FATAL";
      break;
  }

  QString txt = format.arg(timestamp, level, msg);
  QTextStream ts(&Logger::logFile);
  ts << txt;
  ts.flush();
}
