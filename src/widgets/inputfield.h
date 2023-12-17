#ifndef INPUTFIELD_H
#define INPUTFIELD_H

#include <QLineEdit>

class InputField : public QLineEdit
{
  Q_OBJECT
public:
  InputField(QWidget* parent = nullptr, QString text = "");
  void setText(QString text);

signals:
  void rename(QString text);

protected:
  void focusInEvent(QFocusEvent* event);
  void focusOutEvent(QFocusEvent* event);

private slots:
  void enterPressed();

private:
  QString m_currText;
};

#endif // INPUTFIELD_H
