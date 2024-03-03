/**
 * @file notificationpopup.cpp
 * @brief Implementation file for NotificationPopup
 */

#include "notificationpopup.h"
#include <QtAwesome.h>
#include <utils/stylemanager.h>
using namespace fa;

NotificationPopup::NotificationPopup(QWidget* parent)
  : QFrame(parent)
  , m_iconWidget(new QLabel(this))
  , m_textWidget(new QLabel(this))
  , m_opacityEffect(new QGraphicsOpacityEffect(this))
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
NotificationPopup::registerSender(NotificationSender* sender)
{
  connect(
    sender, &NotificationSender::notify, this, &NotificationPopup::notify);
}

void
NotificationPopup::setStyle(NotificationType type)
{
  if (type == NotificationSender::fail)
    setStyleSheet("QFrame#Popup { background-color: #a50500 }");
  else
    setStyleSheet("");
}

void
NotificationPopup::setDockArea(Qt::DockWidgetArea dockPos)
{
  m_dockArea = dockPos;
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
NotificationPopup::setNotificationIcon(NotificationType type)
{
  QString ico;
  int faStyle = fa_solid;
  switch (type) {
    case NotificationSender::info:
      ico = fa_info_circle;
      break;
    case NotificationSender::success:
      ico = fa_check_circle;
      break;
    case NotificationSender::fail:
      ico = fa_xmark_circle;
      break;
    case NotificationSender::bookmarkAdd:
      ico = fa_bookmark;
      break;
    case NotificationSender::bookmarkRemove:
      ico = fa_bookmark;
      faStyle = fa_regular;
      break;
    case NotificationSender::copiedText:
      ico = fa_clipboard;
      break;
    case NotificationSender::updateInfo:
      ico = fa_circle_up;
      break;
  }

  m_iconWidget->setFont(
    StyleManager::getInstance().awesome().font(faStyle, 18));
  m_iconWidget->setText(ico);
}

void
NotificationPopup::notify(NotificationType type, QString message)
{
  QFontMetrics fm(m_textWidget->fontMetrics());
  m_textWidget->setText(message);
  setNotificationIcon(type);
  setStyle(type);

  resize(fm.size(Qt::TextSingleLine, message).width() + 50, 40);
  adjustLocation();
  move(m_notificationPos);
  this->show();

  if (m_fadeoutAnim->state() == QAbstractAnimation::Running)
    m_fadeoutAnim->stop();
  m_opacityEffect->setOpacity(1);
  m_notificationPeriod.start();
}

QPoint
NotificationPopup::notificationPos() const
{
  return m_notificationPos;
}
