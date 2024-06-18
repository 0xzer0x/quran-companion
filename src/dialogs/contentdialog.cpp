/**
 * @file tafsirdialog.cpp
 * @brief Implementation file for ContentDialog
 */

#include "contentdialog.h"
#include "ui_contentdialog.h"
#include <types/tafsir.h>
#include <utils/fontmanager.h>
#include <service/servicefactory.h>
#include <utils/stylemanager.h>

ContentDialog::ContentDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::ContentDialog)
  , m_config(Configuration::getInstance())
  , m_quranService(ServiceFactory::quranService())
  , m_bookmarkService(ServiceFactory::bookmarkService())
  , m_glyphService(ServiceFactory::glyphService())
  , m_tafsirService(ServiceFactory::tafsirService())
  , m_translationService(ServiceFactory::translationService())
  , m_thoughtsService(ServiceFactory::thoughtsService())
  , m_tafasir(Tafsir::tafasir)
  , m_translations(Translation::translations)
  , m_currMode(Tafsir)
  , m_internalLoading(false)
{
  setWindowIcon(
    StyleManager::getInstance().awesome().icon(fa::fa_solid, fa::fa_book_open));
  ui->setupUi(this);
  ui->frmNav->setLayoutDirection(Qt::LeftToRight);
  ui->btnNext->setIcon(StyleManager::getInstance().awesome().icon(
    fa::fa_solid, fa::fa_arrow_left));
  ui->btnPrev->setIcon(StyleManager::getInstance().awesome().icon(
    fa::fa_solid, fa::fa_arrow_right));

  if (m_config.qcfVersion() == 1)
    m_fontSZ = 18;
  else
    m_fontSZ = 16;

  m_tafsir = m_config.settings().value("Reader/Tafsir").toInt();
  m_translation = m_config.settings().value("Reader/Translation").toInt();
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
ContentDialog::showVerseTafsir(const Verse& v)
{
  static bool reload = false;
  if (reload) {
      m_tafsirService->loadTafsir();
    reload = false;
  }

  if (!m_tafsirService->currTafsir()->isAvailable()) {
    int i = m_tafasir.indexOf(*m_tafsirService->currTafsir());
    reload = true;
    emit missingTafsir(i);
    return;
  }

  setShownVerse(v);
  loadContent(Mode::Tafsir);
  show();
}

void
ContentDialog::showVerseTranslation(const Verse& v)
{
  static bool reload = false;
  if (reload) {
      m_translationService->loadTranslation();
    reload = false;
  }

  if (!m_translationService->currTranslation()->isAvailable()) {
    int i = m_translations.indexOf(*m_translationService->currTranslation());
    reload = true;
    emit missingTranslation(i);
    return;
  }

  setShownVerse(v);
  loadContent(Mode::Translation);
  show();
}

void
ContentDialog::showVerseThoughts(const Verse& v)
{
  setShownVerse(v);
  loadContent(Mode::Thoughts);
  show();
}

void
ContentDialog::loadSideFont()
{
  QFont sideFont =
    qvariant_cast<QFont>(m_config.settings().value("Reader/SideContentFont"));
  ui->tedContent->setFont(sideFont);
}

void
ContentDialog::setShownVerse(const Verse& newShownVerse)
{
  m_shownVerse = newShownVerse;
  if (m_shownVerse.number() == 0)
    m_shownVerse.setNumber(1);

  QString title = tr("Surah: ") +
                  m_quranService->surahName(m_shownVerse.surah()) + " - " +
                  tr("Verse: ") + QString::number(m_shownVerse.number());
  QString glyphs =
    m_config.verseType() == Configuration::Qcf
      ? m_glyphService->getVerseGlyphs(m_shownVerse.surah(),
                                       m_shownVerse.number())
      : m_quranService->verseText(m_shownVerse.surah(), m_shownVerse.number());
  QString fontFamily = FontManager::getInstance().getInstance().verseFontname(
    m_config.verseType(), m_shownVerse.page());

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
  setShownVerse(m_quranService->next(m_shownVerse, false));
  loadContent(m_currMode);
}

void
ContentDialog::btnPrevClicked()
{
  setShownVerse(m_quranService->previous(m_shownVerse, false));
  loadContent(m_currMode);
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
  m_tafsir = ui->cmbContent->currentData().toInt();
  m_config.settings().setValue("Reader/Tafsir", m_tafsir);
  if (m_tafsirService->setCurrentTafsir(m_tafsir))
    loadVerseTafsir();
}

void
ContentDialog::translationChanged()
{
  m_translation = ui->cmbContent->currentData().toInt();
  loadVerseTranslation();
}

void
ContentDialog::loadContent(Mode mode)
{
  m_internalLoading = true;
  loadSideFont();
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
    if (m_tafasir.at(i).isAvailable())
      ui->cmbContent->addItem(m_tafasir.at(i).displayName(), i);
  }

  int idx = ui->cmbContent->findData(m_tafsir);
  ui->cmbContent->setCurrentIndex(idx);
}

void
ContentDialog::cmbLoadTranslations()
{
  for (int i = 0; i < m_translations.size(); i++) {
    if (m_translations.at(i).isAvailable())
      ui->cmbContent->addItem(m_translations.at(i).displayName(), i);
  }

  int idx = ui->cmbContent->findData(m_translation);
  ui->cmbContent->setCurrentIndex(idx);
}

void
ContentDialog::loadVerseTafsir()
{
  if (m_tafsirService->currTafsir()->isText())
    ui->tedContent->setText(
      m_tafsirService->getTafsir(m_shownVerse.surah(), m_shownVerse.number()));
  else
    ui->tedContent->setHtml(
      m_tafsirService->getTafsir(m_shownVerse.surah(), m_shownVerse.number()));
}

void
ContentDialog::loadVerseTranslation()
{
  m_translationService->setCurrentTranslation(m_translation);
  ui->tedContent->setText(m_translationService->getTranslation(
    m_shownVerse.surah(), m_shownVerse.number()));
}

void
ContentDialog::loadVerseThoughts()
{
  ui->tedContent->setText(m_thoughtsService->getThoughts(m_shownVerse));
  ui->tedContent->setReadOnly(false);
  ui->tedContent->setCursorWidth(1);
}

void
ContentDialog::saveVerseThoughts()
{
  ui->tedContent->setCursorWidth(0);
  ui->tedContent->setReadOnly(true);
  m_thoughtsService->saveThoughts(m_shownVerse, ui->tedContent->toPlainText());
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
