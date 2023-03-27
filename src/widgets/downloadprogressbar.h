#ifndef DOWNLOADPROGRESSBAR_H
#define DOWNLOADPROGRESSBAR_H

#include <QProgressBar>

class DownloadProgressBar : public QProgressBar
{
public:
    DownloadProgressBar(QWidget *parent = nullptr, int max = 286);

public slots:
    void updateProgress(qint64 downloaded, qint64 total);
};

#endif // DOWNLOADPROGRESSBAR_H
