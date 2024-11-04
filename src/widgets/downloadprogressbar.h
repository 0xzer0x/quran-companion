/**
 * @file downloadprogressbar.h
 * @brief Header file for DownloadProgressBar
 */

#ifndef DOWNLOADPROGRESSBAR_H
#define DOWNLOADPROGRESSBAR_H

#include <QProgressBar>
#include <downloader/downloadjob.h>
typedef DownloadJob::Type Type;

/**
 * @brief DownloadProgressBar class is a modified QProgressBar to change its
 * stylesheet based on different states
 */
class DownloadProgressBar : public QProgressBar
{
  Q_OBJECT
public:
  /**
   * @brief class constructor
   * @param parent - pointer to parent widget
   * @param max - maximum value for the progress bar (defaults to longest surah
   * in the Quran)
   */
  DownloadProgressBar(QWidget* parent, Type type, int max);
  /**
   * @brief The State enum represents the different states of the progressbar UI
   * component
   */
  enum State
  {
    downloading,
    completed,
    aborted
  };

public slots:
  void setStyling(State);
  void updateProgress(QSharedPointer<DownloadJob> job);
  void finished();
  void failed();

private:
  Type m_type;
};

#endif // DOWNLOADPROGRESSBAR_H
