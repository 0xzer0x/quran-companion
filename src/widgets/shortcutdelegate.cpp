#include "shortcutdelegate.h"

QWidget*
ShortcutDelegate::createEditor(QWidget* parent,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index) const
{
  if (index.column() != 1)
    return nullptr;
  QKeySequenceEdit* editor = new QKeySequenceEdit(parent);
  connect(editor,
          &QKeySequenceEdit::editingFinished,
          this,
          &ShortcutDelegate::commitAndCloseEditor);
  return editor;
}

void
ShortcutDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  if (index.column() != 1)
    return;

  QKeySequenceEdit* keyEditor = qobject_cast<QKeySequenceEdit*>(editor);
  keyEditor->setKeySequence(QKeySequence(index.data().toString()));
}

void
ShortcutDelegate::setModelData(QWidget* editor,
                               QAbstractItemModel* model,
                               const QModelIndex& index) const
{
  if (index.column() != 1)
    return;

  QKeySequenceEdit* keyEditor = qobject_cast<QKeySequenceEdit*>(editor);
  QStandardItemModel* itemModel = qobject_cast<QStandardItemModel*>(model);
  QString keySeq = keyEditor->keySequence().toString();
  bool available = itemModel->findItems(keySeq, Qt::MatchExactly, 1).empty();
  if (available)
    itemModel->setData(index, keySeq);
  else if (index.data().toString() != keySeq) {
    m_errorBox->setModal(true);
    m_errorBox->show();
  }
}

void
ShortcutDelegate::commitAndCloseEditor()
{
  QKeySequenceEdit* editor = qobject_cast<QKeySequenceEdit*>(sender());
  emit commitData(editor);
  emit closeEditor(editor);
}
