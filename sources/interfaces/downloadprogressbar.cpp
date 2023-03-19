#include "../../headers/interfaces/downloadprogressbar.h"

DownloadProgressBar::DownloadProgressBar(QWidget *parent, int max)
    : QProgressBar(parent)
{
    setFormat("%v / %m");
    setMaximum(max);
    setValue(0);
}

void DownloadProgressBar::updateProgress(qint64 downloaded, qint64 total)
{
    if (maximum() != total)
        setMaximum(total);

    setValue(downloaded);

    //    if (downloaded == total) {
    //        delete parent();
    //    }
}

