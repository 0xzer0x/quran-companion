/**
 * @file clickablelabel.h
 * @brief Header file for ClickableLabel
 */

#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

/**
 * @brief ClickableLabel class is a modified QLabel to send a signal when
 * clicked
 */
class ClickableLabel : public QLabel
{
  Q_OBJECT

public:
  /**
   * @brief class constructor
   * @param parent - pointer to parent widget
   */
  explicit ClickableLabel(QWidget* parent = nullptr);

signals:
  void clicked();

protected:
  void mousePressEvent(QMouseEvent* event);
};

#endif // CLICKABLELABEL_H
