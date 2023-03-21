#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

namespace Ui {
class SearchDialog;
}

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = nullptr);
    ~SearchDialog();

private:
    Ui::SearchDialog *ui;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // SEARCHDIALOG_H
