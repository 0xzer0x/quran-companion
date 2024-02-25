#ifndef TASKDOWNLOADER_H
#define TASKDOWNLOADER_H

#include "interfaces/downloadtask.h"
#include <QNetworkReply>
#include <QObject>
#include <QTime>

class TaskDownloader : public QObject
{
  Q_OBJECT
public:
  explicit TaskDownloader(QObject* parent);
  ~TaskDownloader();

  void process(DownloadTask* task, QNetworkAccessManager* manager);
  void cancel();

  int bytes() const;
  int total() const;

signals:
  void downloadSpeedUpdated(int speed, QString unit);
  void progressed(qint64 bytes, qint64 total);
  void taskError();
  void completed();

private slots:
  void taskProgress(qint64 bytes, qint64 total);
  void handleError(QNetworkReply::NetworkError err);
  bool saveFile();
  void finish();

private:
  QTime m_startTime;
  DownloadTask* m_task;
  QNetworkReply* m_reply;
  int m_bytes;
  int m_total;
};

#endif // TASKDOWNLOADER_H
