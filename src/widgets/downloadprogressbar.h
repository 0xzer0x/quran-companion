#ifndef DOWNLOADPROGRESSBAR_H
#define DOWNLOADPROGRESSBAR_H

#include <QProgressBar>

class DownloadProgressBar : public QProgressBar
{

public:
  DownloadProgressBar(QWidget* parent = nullptr, int max = 286);
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
  QString m_defStylesheet =
    "QProgressBar {text-align: center; color:palette(text); "
    "border-radius: 2px; border: 1px "
    "solid palette(dark); }";
};

#endif // DOWNLOADPROGRESSBAR_H
