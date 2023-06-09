#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget *parent = nullptr);

signals:
    void clicked();

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // CLICKABLELABEL_H
