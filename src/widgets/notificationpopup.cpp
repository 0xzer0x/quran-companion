/**
 * @file notificationpopup.cpp
 * @brief Implementation file for NotificationPopup
 */

#include "notificationpopup.h"

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
  m_iconWidget->setScaledContents(true);
  m_iconWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  m_textWidget->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  m_textWidget->setWordWrap(true);

  QHBoxLayout* lyt = new QHBoxLayout(this);
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
          &NotificationPopup::hide,
          Qt::UniqueConnection);
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
NotificationPopup::completedDownload(int reciterIdx, int surah)
{
  setStyleSheet("");
  QString msg = tr("Download Completed") + ": " +
                m_recitersList.at(reciterIdx).displayName + " - " +
                tr("Surah") + " " + m_dbMgr->surahNameList().at(surah - 1);
  this->notify(msg, success);
}

void
NotificationPopup::downloadError(int reciterIdx, int surah)
{
  setStyleSheet("QFrame#Popup { background-color: #a50500 }");
  QString msg = tr("Download Failed") + ": " +
                m_recitersList.at(reciterIdx).displayName + " - " +
                tr("Surah") + " " + m_dbMgr->surahNameList().at(surah - 1);
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
  QPixmap image;
  switch (icon) {
    case NotificationPopup::info:
      image = qApp->style()->standardPixmap(QStyle::SP_MessageBoxInformation);
      break;
    case NotificationPopup::success:
      image = QPixmap(m_resources.filePath("icons/success.png"));
      break;
    case NotificationPopup::fail:
      image = QPixmap(m_resources.filePath("icons/exit.png"));
      break;
    case NotificationPopup::bookmarkAdd:
      image = QPixmap(m_resources.filePath("icons/bookmark-true.png"));
      break;
    case NotificationPopup::bookmarkRemove:
      image = QPixmap(m_resources.filePath("icons/bookmark-false.png"));
      break;
    case NotificationPopup::copiedText:
      image = QPixmap(m_resources.filePath("icons/copy.png"));
      break;
    case NotificationPopup::updateInfo:
      image = QPixmap(m_resources.filePath("icons/update.png"));
      break;
  }

  m_iconWidget->setPixmap(image.scaledToWidth(24, Qt::SmoothTransformation));
}

QPoint
NotificationPopup::notificationPos() const
{
  return m_notificationPos;
}
