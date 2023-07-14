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
  fnt.setPointSize(10);
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
}

void
NotificationPopup::notify(QString message, NotificationPopup::Type icon)
{
  QFontMetrics fm(m_textWidget->fontMetrics());
  m_textWidget->setText(message);
  setNotificationIcon(icon);
  resize(fm.size(Qt::TextSingleLine, message).width() + 50, 40);
  move(parentWidget()->width() - this->width() - 5, 30);
  this->show();
  m_opacityEffect->setOpacity(1);
  m_notificationPeriod.start();
}

void
NotificationPopup::notifyCompletedDownload(int reciterIdx, int surah)
{
  qInfo() << "Fired off slot";
  QString msg = tr("Download Completed") + ": " +
                m_recitersList.at(reciterIdx).displayName + " - " +
                tr("Surah") + " " + m_dbMgr->surahNameList().at(surah - 1);
  this->notify(msg, downloads);
}

void
NotificationPopup::setNotificationIcon(Type icon)
{
  QPixmap image;
  switch (icon) {
    case NotificationPopup::info:
      image = qApp->style()->standardPixmap(QStyle::SP_MessageBoxInformation);
      break;
    case NotificationPopup::downloads:
      image = QPixmap(m_resources.filePath("icons/download-manager.png"));
      break;
    case NotificationPopup::bookmarks:
      image = QPixmap(m_resources.filePath("icons/bookmark-true.png"));
      break;
  }

  m_iconWidget->setPixmap(image.scaledToWidth(24, Qt::SmoothTransformation));
}
