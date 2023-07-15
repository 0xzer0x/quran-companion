#ifndef NOTIFICATIONPOPUP_H
#define NOTIFICATIONPOPUP_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include <QApplication>
#include <QDockWidget>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QStyle>
#include <QTimer>
#include <QWidget>

class NotificationPopup : public QFrame
{
  Q_OBJECT
public:
  enum Action
  {
    info,
    success,
    fail,
    bookmarkAdd,
    bookmarkRemove,
    copiedText,
    updateInfo
  };
  explicit NotificationPopup(QWidget* parent = nullptr,
                             DBManager* dbMgr = nullptr);
  void notify(QString message, Action icon);

  void adjustLocation();
  QPoint notificationPos() const;

public slots:
  void dockLocationChanged(Qt::DockWidgetArea dockPos);
  void completedDownload(int reciterIdx, int surah);
  void downloadError(int reciterIdx, int surah);
  void bookmarkAdded();
  void bookmarkRemoved();
  void copiedToClipboard();
  void checkUpdate(QString appVer);

private:
  const QDir& m_resources = g_themeResources;
  const QList<Reciter>& m_recitersList = g_recitersList;
  void setNotificationIcon(Action icon);
  DBManager* m_dbMgr;
  QDockWidget* m_dockPtr;
  QLabel* m_iconWidget;
  QLabel* m_textWidget;
  QPropertyAnimation* m_fadeoutAnim;
  QGraphicsOpacityEffect* m_opacityEffect;
  Qt::DockWidgetArea m_dockArea;
  QPoint m_notificationPos;
  QTimer m_notificationPeriod;
};

#endif // NOTIFICATIONPOPUP_H
