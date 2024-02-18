/**
 * @file tafsirdialog.cpp
 * @brief Implementation file for ContentDialog
 */

#include "contentdialog.h"
#include "../types/tafsir.h"
#include "../utils/fontmanager.h"
#include "../utils/stylemanager.h"
#include "ui_contentdialog.h"

ContentDialog::ContentDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::ContentDialog)
{
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
ContentDialog::setupConnections()
{
  connect(
    ui->btnNext, &QPushButton::clicked, this, &ContentDialog::btnNextClicked);
  connect(
    ui->btnPrev, &QPushButton::clicked, this, &ContentDialog::btnPrevClicked);
  connect(ui->cmbType,
          &QComboBox::currentIndexChanged,
          this,
          &ContentDialog::typeChanged);
  connect(ui->cmbContent,
          &QComboBox::currentIndexChanged,
          this,
          &ContentDialog::contentChanged);
}

void
ContentDialog::setSideFont()
{
  QFont sideFont =
    qvariant_cast<QFont>(m_settings->value("Reader/SideContentFont"));
  ui->tedContent->setFont(sideFont);
}

void
ContentDialog::setShownVerse(const Verse& newShownVerse)
{
  m_shownVerse = newShownVerse;

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
ContentDialog::btnNextClicked()
{
  setShownVerse(m_shownVerse.next());
  loadVerseTafsir();
}

void
ContentDialog::btnPrevClicked()
{
  if (m_shownVerse.number() == 1)
    m_shownVerse.setNumber(0);

  setShownVerse(m_shownVerse.prev());
  loadVerseTafsir();
}

void
ContentDialog::typeChanged()
{
  if (m_internalLoading)
    return;

  if (m_currMode == Mode::Thoughts)
    saveVerseThoughts();
  loadContent((Mode)ui->cmbType->currentIndex());
}

void
ContentDialog::contentChanged()
{
  if (m_internalLoading)
    return;

  switch (m_currMode) {
    case Mode::Tafsir:
      tafsirChanged();
      break;
    case Mode::Translation:
      translationChanged();
      break;
    default:
      break;
  }
}

void
ContentDialog::tafsirChanged()
{
  int tafsirIdx = ui->cmbContent->currentData().toInt();
  m_settings->setValue("Reader/Tafsir", tafsirIdx);
  if (m_dbMgr->setCurrentTafsir(tafsirIdx))
    loadVerseTafsir();
}

void
ContentDialog::translationChanged()
{
  int trIdx = ui->cmbContent->currentData().toInt();
  if (m_dbMgr->setCurrentTranslation(trIdx))
    loadVerseTranslation();
}

void
ContentDialog::loadContent(Mode mode)
{
  m_internalLoading = true;
  setSideFont();
  ui->cmbType->setCurrentIndex((int)mode);
  updateContentComboBox(mode);
  switch (mode) {
    case Mode::Tafsir:
      loadVerseTafsir();
      break;
    case Mode::Translation:
      loadVerseTranslation();
      break;
    case Mode::Thoughts:
      loadVerseThoughts();
      break;
  }
  m_currMode = mode;
  m_internalLoading = false;
}

void
ContentDialog::updateContentComboBox(Mode mode)
{
  ui->cmbContent->clear();
  switch (mode) {
    case Mode::Tafsir:
      ui->cmbContent->setDisabled(false);
      cmbLoadTafasir();
      break;
    case Mode::Translation:
      ui->cmbContent->setDisabled(false);
      cmbLoadTranslations();
      break;
    case Mode::Thoughts:
      ui->cmbContent->setDisabled(true);
      break;
  }
}

void
ContentDialog::cmbLoadTafasir()
{
  for (int i = 0; i < m_tafasir.size(); i++) {
    const QSharedPointer<::Tafsir>& t = m_tafasir.at(i);
    if (Tafsir::tafsirExists(t))
      ui->cmbContent->addItem(t->displayName(), i);
  }

  int tafsirIdx = ui->cmbContent->findData(m_settings->value("Reader/Tafsir"));
  ui->cmbContent->setCurrentIndex(tafsirIdx);
}

void
ContentDialog::cmbLoadTranslations()
{
  for (int i = 0; i < m_translations.size(); i++) {
    const QSharedPointer<::Translation>& tr = m_translations[i];
    if (Translation::translationExists(tr))
      ui->cmbContent->addItem(tr->displayName(), i);
  }

  int trIdx = ui->cmbContent->findData(m_settings->value("Reader/Translation"));
  ui->cmbContent->setCurrentIndex(trIdx);
}

void
ContentDialog::loadVerseTafsir()
{
  if (m_dbMgr->currTafsir()->isText())
    ui->tedContent->setText(
      m_dbMgr->getTafsir(m_shownVerse.surah(), m_shownVerse.number()));
  else
    ui->tedContent->setHtml(
      m_dbMgr->getTafsir(m_shownVerse.surah(), m_shownVerse.number()));
}

void
ContentDialog::loadVerseTranslation()
{
  ui->tedContent->setText(
    m_dbMgr->getTranslation(m_shownVerse.surah(), m_shownVerse.number()));
}

void
ContentDialog::loadVerseThoughts()
{
  ui->tedContent->setText(m_dbMgr->getThoughts(m_shownVerse.toList()));
  ui->tedContent->setReadOnly(false);
  ui->tedContent->setCursorWidth(1);
}

void
ContentDialog::saveVerseThoughts()
{
  ui->tedContent->setCursorWidth(0);
  ui->tedContent->setReadOnly(true);
  m_dbMgr->saveThoughts(m_shownVerse.toList(), ui->tedContent->toPlainText());
}

void
ContentDialog::closeEvent(QCloseEvent* event)
{
  if (m_currMode == Mode::Thoughts)
    saveVerseThoughts();
  this->hide();
}

ContentDialog::~ContentDialog()
{
  delete ui;
}
