#ifndef DOWNLOADJOB_H
#define DOWNLOADJOB_H

#include <QObject>

class DownloadJob : public QObject
{
  Q_OBJECT
public:
  enum Type
  {
    TafsirFile,
    TranslationFile,
    Qcf,
    Recitation
  };
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual bool isDownloading() = 0;
  virtual int completed() = 0;
  virtual int total() = 0;
  virtual Type type() = 0;
  virtual QString name() = 0;

signals:
  void aborted();
  void finished();
  void progressed();
  void failed();
  void downloadSpeedUpdated(int speed, QString unit);
};

#endif // DOWNLOADJOB_H
