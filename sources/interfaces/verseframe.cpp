#include "../../headers/interfaces/verseframe.h"

VerseFrame::VerseFrame(QWidget *parent)
    : QFrame{parent}
{}

void VerseFrame::highlightFrame()
{
    setStyleSheet(QString("QFrame#%0 "
                          "{background-color:rgba(0, 161, 185, 50);"
                          "border-radius:4px}")
                      .arg(objectName()));
}

void VerseFrame::enterEvent(QEnterEvent *event)
{
    if (styleSheet().isEmpty()) {
        setStyleSheet(QString("QFrame#%0 "
                              "{background-color:rgba(0, 161, 185, 20);"
                              "border-radius:4px}")
                          .arg(objectName()));
    }
}

void VerseFrame::leaveEvent(QEvent *event)
{
    if (!styleSheet().contains("50")) {
        setStyleSheet("");
    }
}
