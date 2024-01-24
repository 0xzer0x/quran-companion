#ifndef SHORTCUTDELEGATE_H
#define SHORTCUTDELEGATE_H

#include <QKeySequenceEdit>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStyledItemDelegate>

class ShortcutDelegate : public QStyledItemDelegate
{
  Q_OBJECT
private:
  QMessageBox* m_errorBox = new QMessageBox(
    QMessageBox::Warning,
    tr("Shortcut Conflict"),
    tr("The key sequence chosen is already reserved, try again"));

private slots:
  void commitAndCloseEditor();

public:
  using QStyledItemDelegate::QStyledItemDelegate;

  QWidget* createEditor(QWidget* parent,
                        const QStyleOptionViewItem& option,
                        const QModelIndex& index) const override;
  void setEditorData(QWidget* editor, const QModelIndex& index) const override;
  void setModelData(QWidget* editor,
                    QAbstractItemModel* model,
                    const QModelIndex& index) const override;
};

#endif // SHORTCUTDELEGATE_H
