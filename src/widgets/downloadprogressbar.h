#ifndef DOWNLOADPROGRESSBAR_H
#define DOWNLOADPROGRESSBAR_H

#include <QProgressBar>

class DownloadProgressBar : public QProgressBar
{
public:
    DownloadProgressBar(QWidget *parent = nullptr, int max = 286);
    enum State { downloading, completed, aborted };

public slots:
    void updateProgress(qint64 downloaded, qint64 total);
    void setStyling(State);

private:
    QString m_defStylesheet;
};

#endif // DOWNLOADPROGRESSBAR_H
