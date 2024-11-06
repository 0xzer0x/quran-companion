/**
 * @file verseframe.h
 * @brief Header file for VerseFrame
 */

#ifndef VERSEFRAME_H
#define VERSEFRAME_H

#include <QApplication>
#include <QFrame>
#include <QPalette>
#include <QVBoxLayout>

/**
 * @brief VerseFrame represents a custom QFrame component surrounding a Quran
 * verse.
 */
class VerseFrame : public QFrame
{
  Q_OBJECT
public:
  /**
   * @brief Class constructor
   * @param parent - pointer to parent widget
   */
  VerseFrame(QWidget* parent = nullptr);

  /**
   * @brief set the VerseFrame::m_selected member and change the stylesheet
   * accordingly.
   * @param selected - selection state
   */
  void setSelected(bool selected);
  void setHighlightColor(int r, int g, int b);

protected:
  /**
   * @brief Re-implementation of QWidget::enterEvent() in order to provide mouse
   * hover interaction.
   * @param event
   */
  void enterEvent(QEnterEvent* event);
  /**
   * @brief Re-implementation of QWidget::leaveEvent() in order to provide mouse
   * hover interaction.
   * @param event
   */
  void leaveEvent(QEvent* event);

private:
  int m_colorR = qApp->palette().color(QPalette::Highlight).red();
  int m_colorG = qApp->palette().color(QPalette::Highlight).green();
  int m_colorB = qApp->palette().color(QPalette::Highlight).blue();
  /**
   * @brief boolean value representing Frame selection state.
   */
  bool m_selected = false;
  /**
   * @brief QString for hovered verse stylesheet rgb color.
   */
  QString m_rgbHover;
  /**
   * @brief QString for selected verse stylesheet rgb color.
   */
  QString m_rgbSelected;
  /**
   * @brief QString for frame stylesheet. Used with m_rgbHover or m_rgbSelected.
   */
  QString m_frameStylesheet;
};

#endif // VERSEFRAME_H
