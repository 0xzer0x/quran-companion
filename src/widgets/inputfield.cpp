#include "inputfield.h"

InputField::InputField(QWidget* parent, QString text)
  : QLineEdit(text, parent)
  , m_currText(text)
{
  setObjectName("InputField");
  setStyleSheet("#InputField { border: none; background: "
                "transparent; color: palette(text); }");

  connect(this, &QLineEdit::returnPressed, this, &InputField::enterPressed);
  setCursor(Qt::CursorShape::ArrowCursor);
}

void
InputField::setText(QString text)
{
  m_currText = text;
  QLineEdit::setText(m_currText);
}

void
InputField::focusInEvent(QFocusEvent* event)
{
  setStyleSheet("#InputField { border: 1px "
                "solid palette(highlight); border-radius: 2px; background: "
                "palette(window); }");
  QLineEdit::focusInEvent(event);
}

void
InputField::focusOutEvent(QFocusEvent* event)
{
  if (text().isEmpty())
    return setFocus();
  setStyleSheet("#InputField { border: none; background: "
                "transparent; color: palette(text); }");
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
