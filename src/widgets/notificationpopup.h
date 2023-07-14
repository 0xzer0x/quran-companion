#ifndef NOTIFICATIONPOPUP_H
#define NOTIFICATIONPOPUP_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include <QApplication>
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
  enum Type
  {
    info,
    downloads,
    bookmarks
  };
  explicit NotificationPopup(QWidget* parent = nullptr,
                             DBManager* dbMgr = nullptr);
  void notify(QString message, Type icon);

public slots:
  void notifyCompletedDownload(int reciterIdx, int surah);

private:
  const QDir& m_resources = g_themeResources;
  const QList<Reciter>& m_recitersList = g_recitersList;
  void setNotificationIcon(Type icon);
  DBManager* m_dbMgr;
  QLabel* m_iconWidget;
  QLabel* m_textWidget;
  QPropertyAnimation* m_fadeoutAnim;
  QGraphicsOpacityEffect* m_opacityEffect;
  QTimer m_notificationPeriod;
};

#endif // NOTIFICATIONPOPUP_H
