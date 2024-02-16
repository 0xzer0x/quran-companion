/**
 * @file notificationpopup.h
 * @brief Header file for NotificationPopup
 */

#ifndef NOTIFICATIONPOPUP_H
#define NOTIFICATIONPOPUP_H

#include "../utils/dbmanager.h"
#include "../utils/downloadmanager.h"
#include <QApplication>
#include <QDockWidget>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QStyle>
#include <QTimer>
#include <QWidget>
typedef DownloadManager::DownloadType DownloadType;

/**
 * @brief NotificationPopup class represents an in-app popup for notifying the
 * user after different actions
 */
class NotificationPopup : public QFrame
{
  Q_OBJECT
public:
  /**
   * @brief The Action enum represents all possible action to notify the user of
   */
  enum Action
  {
    info,           ///< general information
    success,        ///< successful operation
    fail,           ///< failed operation
    bookmarkAdd,    ///< bookmark addition
    bookmarkRemove, ///< bookmark removal
    copiedText,     ///< verse text copied
    updateInfo      ///< version information
  };

  /**
   * @brief class constructor
   * @param parent - pointer to parent widget
   * @param dbMgr - pointer to DBManager instance
   */
  explicit NotificationPopup(QWidget* parent = nullptr);
  /**
   * @brief show popup with the given message and action icon
   * @param message - QString of message to show
   * @param icon - NotificationPopup::Action entry
   */
  void notify(QString message, Action icon);

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
  /**
   * @brief slot to show a notification on download completion
   * @param reciterIdx - ::Globals::recitersList index for the reciter
   * @param surah - the surah that was downloaded
   */
  void completedDownload(DownloadType type, const QList<int>& metainfo);
  /**
   * @brief slot to show a notification on download error
   * @param reciterIdx - ::Globals::recitersList index for the reciter
   * @param surah - the surah that was downloaded
   */
  void downloadError(DownloadType type, const QList<int>& metainfo);
  /**
   * @brief slot to show a notification on bookmark addition
   */
  void bookmarkAdded();
  /**
   * @brief slot to show a notification on bookmark removal
   */
  void bookmarkRemoved();
  /**
   * @brief slot to show a notification after verse text is copied to clipboard
   */
  void copiedToClipboard();
  /**
   * @brief slot to check the passed version against the application version and
   * show notification accordingly
   * @param appVer - the fetched application version
   */
  void checkUpdate(QString appVer);

private:
  QSharedPointer<DBManager> m_dbMgr = DBManager::current();
  QList<QSharedPointer<Reciter>>& m_recitersList = Reciter::reciters;
  QList<QSharedPointer<Tafsir>>& m_tafasir = Tafsir::tafasir;
  QList<QSharedPointer<Translation>>& m_trList = Translation::translations;
  /**
   * @brief connects signals and slots for different UI
   * components and shortcuts.
   */
  void setupConnections();
  /**
   * @brief set the popup icon according to the given Action
   * @param icon - NotificationPopup::Action entry
   */
  void setNotificationIcon(Action icon);
  QLabel* m_iconWidget;
  QLabel* m_textWidget;
  QPropertyAnimation* m_fadeoutAnim;
  QGraphicsOpacityEffect* m_opacityEffect;
  Qt::DockWidgetArea m_dockArea;
  QPoint m_notificationPos;
  QTimer m_notificationPeriod;
};

#endif // NOTIFICATIONPOPUP_H
