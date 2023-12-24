#include "inputfield.h"

InputField::InputField(QWidget* parent, QString text)
  : QLineEdit(text, parent)
  , m_currText(text)
{
  setObjectName("InputField");
  connect(this, &QLineEdit::returnPressed, this, &InputField::enterPressed);
}

void
InputField::setText(QString text)
{
  m_currText = text;
  QLineEdit::setText(m_currText);
}

void
InputField::focusOutEvent(QFocusEvent* event)
{
  if (text().isEmpty())
    return setFocus();
  QLineEdit::setText(m_currText);
  QLineEdit::focusOutEvent(event);
}

void
InputField::enterPressed()
{
  if (!text().isEmpty()) {
    emit rename(this->text());
  }
}
