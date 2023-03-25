#ifndef VERSEFRAME_H
#define VERSEFRAME_H

#include <QFrame>
#include <QVBoxLayout>

class VerseFrame : public QFrame
{
public:
    VerseFrame(QWidget *parent = nullptr);
    void highlightFrame();

    // QWidget interface
protected:
    void enterEvent(QEnterEvent *event);
    void leaveEvent(QEvent *event);
};

#endif // VERSEFRAME_H
