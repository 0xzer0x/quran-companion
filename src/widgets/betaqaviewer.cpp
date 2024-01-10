#include "betaqaviewer.h"
#include "ui_betaqaviewer.h"
#include <QScrollBar>

BetaqaViewer::BetaqaViewer(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::BetaqaViewer)
{
  ui->setupUi(this);
  this->hide();
  ui->betaqaTextBrowser->verticalScrollBar()->setVisible(false);
}

void
BetaqaViewer::showSurah(int surah)
{
  if (surah != m_surah) {
    ui->betaqaTextBrowser->setHtml(m_dbMgr->getBetaqa(surah));
    m_surah = surah;
  }

  this->show();
  this->setFocus();
}

void
BetaqaViewer::center()
{
  int w = std::min((int)(parentWidget()->width() * 0.7), 800);
  this->resize(w, w);

  QPoint center(((parentWidget()->width() / 2) - (w / 2)),
                ((parentWidget()->height() / 2) - (w / 2)));
  move(center);
}

void
BetaqaViewer::focusOutEvent(QFocusEvent* event)
{
  this->hide();
}

BetaqaViewer::~BetaqaViewer()
{
  delete ui;
}
