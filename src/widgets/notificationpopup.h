/**
 * @file notificationpopup.h
 * @brief Header file for NotificationPopup
 */

#ifndef NOTIFICATIONPOPUP_H
#define NOTIFICATIONPOPUP_H

#include <QApplication>
#include <QDockWidget>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPointer>
#include <QPropertyAnimation>
#include <QStyle>
#include <QTimer>
#include <QWidget>
#include <interfaces/downloadjob.h>
#include <interfaces/notificationsender.h>
#include <types/reciter.h>
#include <types/tafsir.h>
#include <types/translation.h>
typedef NotificationSender::Type NotificationType;

/**
 * @brief NotificationPopup class represents an in-app popup for notifying the
 * user after different actions
 */
class NotificationPopup : public QFrame
{
  Q_OBJECT
public:
  /**
   * @brief class constructor
   * @param parent - pointer to parent widget
   * @param dbMgr - pointer to DBManager instance
   */
  explicit NotificationPopup(QWidget* parent = nullptr);
  /**
   * @brief registerSender
   * @param sender
   *
   * MODIFIED
   */
  void registerSender(NotificationSender* sender);
  /**
   * @brief adjust the popup position based on the position of the side dock
   * position in the main window
   * @details sets the popup position to be on the opposite side of the side
   * dock
   */
  void adjustLocation();
  /**
   * @brief getter for m_notificationPos
   * @return QPoint representing the (0,0) coordinate for the widget
   */
  QPoint notificationPos() const;

public slots:
  /**
   * @brief slot to update m_dockArea variable on dock position change
   * @param dockPos - new dock position relative to the main window
   */
  void setDockArea(Qt::DockWidgetArea dockPos);

private slots:
  /**
   * @brief show popup with the given message and action icon
   * @param message - QString of message to show
   * @param icon - NotificationPopup::Action entry
   */
  void notify(NotificationType icon, QString message);

private:
  /**
   * @brief connects signals and slots for different UI
   * components and shortcuts.
   */
  void setupConnections();
  /**
   * @brief setStyle
   * @param type
   */
  void setStyle(NotificationType type);
  /**
   * @brief set the popup icon according to the given Action
   * @param icon - NotificationPopup::Action entry
   */
  void setNotificationIcon(NotificationType type);
  QPointer<QLabel> m_iconWidget;
  QPointer<QLabel> m_textWidget;
  QPointer<QPropertyAnimation> m_fadeoutAnim;
  QPointer<QGraphicsOpacityEffect> m_opacityEffect;
  Qt::DockWidgetArea m_dockArea;
  QPoint m_notificationPos;
  QTimer m_notificationPeriod;
};

#endif // NOTIFICATIONPOPUP_H
