/**
 * @file downloadprogressbar.h
 * @brief Header file for DownloadProgressBar
 */

#ifndef DOWNLOADPROGRESSBAR_H
#define DOWNLOADPROGRESSBAR_H

#include <QProgressBar>

/**
 * @brief DownloadProgressBar class is a modified QProgressBar to change its
 * stylesheet based on different states
 */
class DownloadProgressBar : public QProgressBar
{
public:
  /**
   * @brief class constructor
   * @param parent - pointer to parent widget
   * @param max - maximum value for the progress bar (defaults to longest surah
   * in the Quran)
   */
  DownloadProgressBar(QWidget* parent, int max);
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
  void updateProgress(qint64 downloaded, qint64 total);
  void setStyling(State);

private:
  QString m_defStylesheet;
};

#endif // DOWNLOADPROGRESSBAR_H
