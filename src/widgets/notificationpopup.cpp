/**
 * @file notificationpopup.cpp
 * @brief Implementation file for NotificationPopup
 */

#include "notificationpopup.h"
using namespace fa;

NotificationPopup::NotificationPopup(QWidget* parent)
  : QFrame{ parent }
  , m_iconWidget{ new QLabel(this) }
  , m_textWidget{ new QLabel(this) }
  , m_opacityEffect{ new QGraphicsOpacityEffect(this) }
{
  setObjectName("Popup");
  this->setGraphicsEffect(m_opacityEffect);
  m_opacityEffect->setOpacity(0);

  m_fadeoutAnim = new QPropertyAnimation(m_opacityEffect, "opacity", this);
  m_fadeoutAnim->setDuration(1500);
  m_fadeoutAnim->setStartValue(1);
  m_fadeoutAnim->setEndValue(0);
  m_fadeoutAnim->setEasingCurve(QEasingCurve::OutBack);

  QFont fnt = m_textWidget->font();
  fnt.setPointSize(12);
  m_textWidget->setFont(fnt);
  m_textWidget->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  m_textWidget->setWordWrap(true);

  m_iconWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  QHBoxLayout* lyt = new QHBoxLayout(this);
  lyt->setSpacing(10);
  lyt->addWidget(m_iconWidget, 0);
  lyt->addWidget(m_textWidget, 1);

  m_notificationPeriod.setSingleShot(true);
  m_notificationPeriod.setInterval(4000);
  connect(&m_notificationPeriod, &QTimer::timeout, m_fadeoutAnim, [this]() {
    if (m_opacityEffect->opacity() == 1)
      m_fadeoutAnim->start();
  });

  this->hide();
  setupConnections();
}

void
NotificationPopup::setupConnections()
{
  connect(m_fadeoutAnim,
          &QPropertyAnimation::finished,
          this,
          &NotificationPopup::hide);
}

void
NotificationPopup::dockLocationChanged(Qt::DockWidgetArea dockPos)
{
  m_dockArea = dockPos;
}

void
NotificationPopup::notify(QString message, NotificationPopup::Action icon)
{
  QFontMetrics fm(m_textWidget->fontMetrics());
  m_textWidget->setText(message);
  setNotificationIcon(icon);

  resize(fm.size(Qt::TextSingleLine, message).width() + 50, 40);
  adjustLocation();
  move(m_notificationPos);
  this->show();

  if (m_fadeoutAnim->state() == QAbstractAnimation::Running)
    m_fadeoutAnim->stop();
  m_opacityEffect->setOpacity(1);
  m_notificationPeriod.start();
}

void
NotificationPopup::completedDownload(DownloadType type,
                                     const QList<int>& metainfo)
{
  QString msg = tr("Download Completed") + ": ";
  setStyleSheet("");
  if (type == Recitation)
    msg += m_recitersList.at(metainfo[0]).displayName + " - " +
           m_dbMgr->surahNameList().at(metainfo[1] - 1);
  else if (type == QCF)
    msg += tr("QCF V2");

  this->notify(msg, success);
}

void
NotificationPopup::downloadError(DownloadType type, const QList<int>& metainfo)
{
  setStyleSheet("QFrame#Popup { background-color: #a50500 }");
  QString msg = tr("Download Failed") + ": ";
  if (type == Recitation)
    msg += m_recitersList.at(metainfo[0]).displayName + " - " +
           m_dbMgr->surahNameList().at(metainfo[1] - 1);

  this->notify(msg, fail);
}

void
NotificationPopup::bookmarkAdded()
{
  setStyleSheet("");
  QString msg = tr("Verse added to bookmarks");
  this->notify(msg, bookmarkAdd);
}

void
NotificationPopup::bookmarkRemoved()
{
  setStyleSheet("");
  QString msg = tr("Verse removed from bookmarks");
  this->notify(msg, bookmarkRemove);
}

void
NotificationPopup::copiedToClipboard()
{
  setStyleSheet("");
  QString msg = tr("Verse text copied to clipboard");
  this->notify(msg, copiedText);
}

void
NotificationPopup::checkUpdate(QString appVer)
{
  if (appVer.isEmpty())
    return;

  QString msg;
  if (qApp->applicationVersion() == appVer) {
    msg = tr("You are running the latest version");
    this->notify(msg, success);
  } else {
    msg = tr("Update available") + ": " + appVer;
    this->notify(msg, updateInfo);
  }
}

void
NotificationPopup::adjustLocation()
{
  m_notificationPos.setY(
    parentWidget()->findChild<QWidget*>("menubar")->height() + 5);

  switch (m_dockArea) {
    case Qt::LeftDockWidgetArea:
      m_notificationPos.setX(parentWidget()->width() - this->width() - 5);
      break;
    case Qt::RightDockWidgetArea:
      m_notificationPos.setX(5);
      break;
    default:
      break;
  }
}

void
NotificationPopup::setNotificationIcon(Action icon)
{
  QString ico;
  int faStyle = fa_solid;
  switch (icon) {
    case NotificationPopup::info:
      ico = fa_info_circle;
      break;
    case NotificationPopup::success:
      ico = fa_check_circle;
      break;
    case NotificationPopup::fail:
      ico = fa_xmark_circle;
      break;
    case NotificationPopup::bookmarkAdd:
      ico = fa_bookmark;
      break;
    case NotificationPopup::bookmarkRemove:
      ico = fa_bookmark;
      faStyle = fa_regular;
      break;
    case NotificationPopup::copiedText:
      ico = fa_clipboard;
      break;
    case NotificationPopup::updateInfo:
      ico = fa_circle_up;
      break;
  }

  m_iconWidget->setFont(Globals::awesome->font(faStyle, 18));
  m_iconWidget->setText(ico);
}

QPoint
NotificationPopup::notificationPos() const
{
  return m_notificationPos;
}
