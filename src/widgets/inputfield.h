#ifndef INPUTFIELD_H
#define INPUTFIELD_H

#include <QLineEdit>

/**
 * @brief InputField is a modified QLineEdit used for renaming attributes
 */
class InputField : public QLineEdit
{
  Q_OBJECT
public:
  /**
   * @brief InputField class constructor
   * @param parent - pointer to parent widget
   * @param text - text to set as the currently approved text
   */
  InputField(QWidget* parent = nullptr, QString text = "");
  /**
   * @brief Setter for m_currText
   * @param text - new approved text
   */
  void setText(QString text);

signals:
  /**
   * @brief signal emitted when the user submits a non-empty text
   * @param text - QString that the user submitted
   */
  void rename(QString text);

protected:
  void focusOutEvent(QFocusEvent* event);

private slots:
  /**
   * @brief handle case when user submits an empty name before sending signal
   */
  void enterPressed();

private:
  QString m_currText;
};

#endif // INPUTFIELD_H
