#ifndef KHATMAHDIALOG_H
#define KHATMAHDIALOG_H

#include <QDialog>

namespace Ui {
class KhatmahDialog;
}

class KhatmahDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KhatmahDialog(QWidget *parent = nullptr);
    ~KhatmahDialog();

private:
    Ui::KhatmahDialog *ui;
};

#endif // KHATMAHDIALOG_H
