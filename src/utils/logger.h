#ifndef LOGGER_H
#define LOGGER_H

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QObject>
#include <QTextStream>

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = nullptr);
    static QString filename;
    static QFile logFile;

    static bool startLogger();
    static void stopLogger();
    static void attach();
    static void handler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};

#endif // LOGGER_H
