/**
 * @file tafsirdialog.cpp
 * @brief Implementation file for TafsirDialog
 */

#include "tafsirdialog.h"
#include "ui_tafsirdialog.h"

TafsirDialog::TafsirDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::TafsirDialog)
{
  setWindowIcon(Globals::awesome->icon(fa::fa_solid, fa::fa_book_open));
  ui->setupUi(this);
  ui->btnNext->setIcon(Globals::awesome->icon(fa::fa_solid, fa::fa_arrow_left));
  ui->btnPrev->setIcon(
    Globals::awesome->icon(fa::fa_solid, fa::fa_arrow_right));

  setTafsirAsTitle();
  setLayoutDirection(Qt::LeftToRight);
  if (m_qcfVer == 1)
    m_fontSZ = 18;
  else
    m_fontSZ = 16;

  // connectors
  setupConnections();
}

void
TafsirDialog::btnNextClicked()
{
  if (m_shownVerse.number == m_dbMgr->getSurahVerseCount(m_shownVerse.surah) &&
      m_shownVerse.surah < 114) {
    m_shownVerse.number = 1;
    m_shownVerse.surah++;
  } else {
    m_shownVerse.number++;
  }

  m_shownVerse.page =
    m_dbMgr->getVersePage(m_shownVerse.surah, m_shownVerse.number);
  loadVerseTafsir();
}

void
TafsirDialog::btnPrevClicked()
{
  if (m_shownVerse.number == 1 && m_shownVerse.surah > 1) {
    m_shownVerse.surah--;
    m_shownVerse.number = m_dbMgr->getSurahVerseCount(m_shownVerse.surah);
  } else {
    m_shownVerse.number--;
  }

  m_shownVerse.page =
    m_dbMgr->getVersePage(m_shownVerse.surah, m_shownVerse.number);
  loadVerseTafsir();
}

void
TafsirDialog::setupConnections()
{
  connect(
    ui->btnNext, &QPushButton::clicked, this, &TafsirDialog::btnNextClicked);
  connect(
    ui->btnPrev, &QPushButton::clicked, this, &TafsirDialog::btnPrevClicked);
}

void
TafsirDialog::setTafsirAsTitle()
{
  QString title = m_dbMgr->currTafsir()->displayName;
  setWindowTitle(title);
}

void
TafsirDialog::loadVerseTafsir()
{
  QString title = tr("Surah: ") + m_dbMgr->getSurahName(m_shownVerse.surah) +
                  " - " + tr("Verse: ") + QString::number(m_shownVerse.number);
  QString glyphs =
    m_dbMgr->getVerseGlyphs(m_shownVerse.surah, m_shownVerse.number);
  QString fontFamily =
      Globals::verseFontname(m_dbMgr->getVerseType(), m_shownVerse.page);

  ui->lbVerseInfo->setText(title);
  ui->lbVerseText->setWordWrap(true);
  ui->lbVerseText->setFont(QFont(fontFamily, m_fontSZ));
  ui->lbVerseText->setText(glyphs);

  QFont sideFont =
    qvariant_cast<QFont>(m_settings->value("Reader/SideContentFont"));
  ui->tedTafsir->setFont(sideFont);

  if (m_dbMgr->currTafsir()->text)
    ui->tedTafsir->setText(
      m_dbMgr->getTafsir(m_shownVerse.surah, m_shownVerse.number));
  else
    ui->tedTafsir->setHtml(
      m_dbMgr->getTafsir(m_shownVerse.surah, m_shownVerse.number));

  if (m_shownVerse.surah == 1 && m_shownVerse.number == 1)
    ui->btnPrev->setDisabled(true);
  else if (m_shownVerse.surah == 114 && m_shownVerse.number == 6)
    ui->btnNext->setDisabled(true);
  else {
    ui->btnPrev->setDisabled(false);
    ui->btnNext->setDisabled(false);
  }
}

void
TafsirDialog::setShownVerse(const Verse& newShownVerse)
{
  m_shownVerse = newShownVerse;
}

void
TafsirDialog::closeEvent(QCloseEvent* event)
{
  this->hide();
}

void
TafsirDialog::showEvent(QShowEvent* event)
{
  setTafsirAsTitle();
  QDialog::showEvent(event);
}

TafsirDialog::~TafsirDialog()
{
  delete ui;
}
