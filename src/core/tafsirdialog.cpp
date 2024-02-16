/**
 * @file tafsirdialog.cpp
 * @brief Implementation file for TafsirDialog
 */

#include "tafsirdialog.h"
#include "../types/tafsir.h"
#include "../utils/fontmanager.h"
#include "../utils/stylemanager.h"
#include "ui_tafsirdialog.h"

TafsirDialog::TafsirDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::TafsirDialog)
{
  setWindowTitle(qApp->translate("SettingsDialog", "Tafsir"));
  setWindowIcon(StyleManager::awesome->icon(fa::fa_solid, fa::fa_book_open));
  ui->setupUi(this);
  ui->frmNav->setLayoutDirection(Qt::LeftToRight);
  ui->btnNext->setIcon(
    StyleManager::awesome->icon(fa::fa_solid, fa::fa_arrow_left));
  ui->btnPrev->setIcon(
    StyleManager::awesome->icon(fa::fa_solid, fa::fa_arrow_right));

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
  m_shownVerse = m_shownVerse.next();
  loadVerseTafsir();
}

void
TafsirDialog::btnPrevClicked()
{
  if (m_shownVerse.number() == 1)
    m_shownVerse.setNumber(0);
  m_shownVerse = m_shownVerse.prev();
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
TafsirDialog::updateContentComboBox()
{
  ui->cmbTafsir->clear();
  for (int i = 0; i < m_tafasirList.size(); i++) {
    const QSharedPointer<Tafsir> t = m_tafasirList.at(i);
    if (Tafsir::tafsirExists(t))
      ui->cmbTafsir->addItem(t->displayName(), i);
  }

  m_tafsir = ui->cmbTafsir->findData(m_settings->value("Reader/Tafsir"));
  ui->cmbTafsir->setCurrentIndex(m_tafsir);
}

void
TafsirDialog::loadVerseTafsir()
{
  if (!m_shownVerse.number())
    m_shownVerse.setNumber(1);

  QString title = tr("Surah: ") + m_dbMgr->getSurahName(m_shownVerse.surah()) +
                  " - " + tr("Verse: ") +
                  QString::number(m_shownVerse.number());
  QString glyphs =
    m_dbMgr->getVerseGlyphs(m_shownVerse.surah(), m_shownVerse.number());
  QString fontFamily =
    FontManager::verseFontname(m_dbMgr->getVerseType(), m_shownVerse.page());

  ui->lbVerseInfo->setText(title);
  ui->lbVerseText->setWordWrap(true);
  ui->lbVerseText->setFont(QFont(fontFamily, m_fontSZ));
  ui->lbVerseText->setText(glyphs);

  QFont sideFont =
    qvariant_cast<QFont>(m_settings->value("Reader/SideContentFont"));
  ui->tedTafsir->setFont(sideFont);

  if (m_dbMgr->currTafsir()->isText())
    ui->tedTafsir->setText(
      m_dbMgr->getTafsir(m_shownVerse.surah(), m_shownVerse.number()));
  else
    ui->tedTafsir->setHtml(
      m_dbMgr->getTafsir(m_shownVerse.surah(), m_shownVerse.number()));

  if (m_shownVerse.surah() == 1 && m_shownVerse.number() == 1)
    ui->btnPrev->setDisabled(true);
  else if (m_shownVerse.surah() == 114 && m_shownVerse.number() == 6)
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
TafsirDialog::showEvent(QShowEvent* event)
{
  updateContentComboBox();
  QDialog::showEvent(event);
}

void
TafsirDialog::closeEvent(QCloseEvent* event)
{
  this->hide();
}

TafsirDialog::~TafsirDialog()
{
  delete ui;
}
