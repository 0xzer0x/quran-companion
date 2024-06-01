#include "betaqaviewer.h"
#include "ui_betaqaviewer.h"
#include <QScrollBar>
#include <qabstractanimation.h>
#include <qnamespace.h>
#include <qpropertyanimation.h>

BetaqaViewer::BetaqaViewer(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::BetaqaViewer)
  , m_shadowEffect(new QGraphicsDropShadowEffect(this))
  , m_sizeAnim(new QPropertyAnimation(this, "size"))
    , m_betaqatDb(BetaqatRepository::getInstance())
{
  this->hide();
  ui->setupUi(this);
  ui->betaqaTextBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->betaqaTextBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->betaqaTextBrowser->setFont(QFont(qApp->font().families(), 14));

  m_shadowEffect->setOffset(2);
  m_shadowEffect->setBlurRadius(22);
  m_shadowEffect->setColor(palette().color(QPalette::Shadow));
  this->setGraphicsEffect(m_shadowEffect);

  m_sizeAnim->setStartValue(QSize(0, 0));
  m_sizeAnim->setEndValue(size());
  m_sizeAnim->setDuration(200);
  m_sizeAnim->setEasingCurve(QEasingCurve::InOutQuad);

  connect(m_sizeAnim, &QPropertyAnimation::finished, this, [this]() {
    if (m_sizeAnim->direction() == QPropertyAnimation::Backward)
      this->hide();
  });
}

void
BetaqaViewer::showSurah(int surah)
{
  if (surah != m_surah) {
    ui->betaqaTextBrowser->setHtml(m_betaqatDb.getBetaqa(surah));
    m_surah = surah;
  }

  this->show();
  m_sizeAnim->setDirection(QAbstractAnimation::Forward);
  m_sizeAnim->start();
  this->setFocus();
  ui->betaqaTextBrowser->verticalScrollBar()->setValue(0);
}

void
BetaqaViewer::center()
{
  int w = std::max((int)(parentWidget()->width() * 0.6), 600);
  int h = std::max((int)(parentWidget()->height() * 0.8), 600);
  this->resize(w, h);
  m_sizeAnim->setEndValue(size());

  QPoint center(((parentWidget()->width() / 2) - (w / 2)),
                ((parentWidget()->height() / 2) - (h / 2)));
  move(center);
}

void
BetaqaViewer::focusOutEvent(QFocusEvent* event)
{
  m_sizeAnim->setDirection(QAbstractAnimation::Backward);
  m_sizeAnim->start();
}

BetaqaViewer::~BetaqaViewer()
{
  delete ui;
}
