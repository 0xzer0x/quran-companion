#include "notificationpopup.h"

NotificationPopup::NotificationPopup(QWidget* parent, DBManager* dbMgr)
  : QFrame{ parent }
  , m_dbMgr{ dbMgr }
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

  this->show();
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

  if (m_fadeoutAnim->state() == QAbstractAnimation::Running)
    m_fadeoutAnim->stop();
  m_opacityEffect->setOpacity(1);
  m_notificationPeriod.start();
}

void
NotificationPopup::completedDownload(int reciterIdx, int surah)
{
  QString msg = tr("Download Completed") + ": " +
                m_recitersList.at(reciterIdx).displayName + " - " +
                tr("Surah") + " " + m_dbMgr->surahNameList().at(surah - 1);
  this->notify(msg, downloads);
}

void
NotificationPopup::bookmarkAdded()
{
  QString msg = tr("Verse added to bookmarks");
  this->notify(msg, bookmarkAdd);
}

void
NotificationPopup::bookmarkRemoved()
{
  QString msg = tr("Verse removed from bookmarks");
  this->notify(msg, bookmarkRemove);
}

void
NotificationPopup::copiedToClipboard()
{
  QString msg = tr("Verse text copied to clipboard");
  this->notify(msg, copiedText);
}

void
NotificationPopup::adjustLocation()
{
  switch (m_dockArea) {
    case Qt::LeftDockWidgetArea:
      m_notificationPos.setX(parentWidget()->width() - this->width() - 5);
      m_notificationPos.setY(30);
      break;

    case Qt::RightDockWidgetArea:
      m_notificationPos.setX(5);
      m_notificationPos.setY(30);
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
    case NotificationPopup::downloads:
      image = QPixmap(m_resources.filePath("icons/download-manager.png"));
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
  }

  m_iconWidget->setPixmap(image.scaledToWidth(24, Qt::SmoothTransformation));
}

QPoint
NotificationPopup::notificationPos() const
{
  return m_notificationPos;
}
