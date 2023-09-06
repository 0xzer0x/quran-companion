/**
 * @file tafsirdialog.cpp
 * @brief Implementation file for TafsirDialog
 */

#include "tafsirdialog.h"
#include "ui_tafsirdialog.h"

TafsirDialog::TafsirDialog(QWidget* parent, DBManager* dbPtr)
  : QDialog(parent)
  , ui(new Ui::TafsirDialog)
  , m_dbMgr{ dbPtr }
{
  setWindowIcon(QIcon(m_resources.filePath("icons/tafsir.png")));
  ui->setupUi(this);
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
TafsirDialog::setShownVerse(const Verse& newShownVerse)
{
  m_shownVerse = newShownVerse;
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
  QShortcut* ctrlQ = new QShortcut(QKeySequence("Ctrl+Q"), this);
  connect(ctrlQ, &QShortcut::activated, this, &TafsirDialog::close);
  connect(ui->btnNext,
          &QPushButton::clicked,
          this,
          &TafsirDialog::btnNextClicked,
          Qt::UniqueConnection);
  connect(ui->btnPrev,
          &QPushButton::clicked,
          this,
          &TafsirDialog::btnPrevClicked,
          Qt::UniqueConnection);
}

void
TafsirDialog::setTafsirAsTitle()
{
  QString title;
  Tafsir id = m_dbMgr->currTafsir();
  switch (id) {
    case Tafsir::adwa:
      title.append(tr("Adwa' ul-Bayan"));
      break;
    case Tafsir::aysar:
      title.append(tr("Aysar Al-Tafasir"));
      break;
    case Tafsir::baghawy:
      title.append(tr("Al-Baghawy"));
      break;
    case Tafsir::e3rab:
      title.append(tr("Earab"));
      break;
    case Tafsir::indonesian:
      title.append(tr("Indonesian - Tafsir Jalalayn"));
      break;
    case Tafsir::juzayy:
      title.append(tr("Ibn-Juzayy"));
      break;
    case Tafsir::katheer:
      title.append(tr("Ibn-Katheer"));
      break;
    case Tafsir::qortoby:
      title.append(tr("Al-Qortoby"));
      break;
    case Tafsir::russian:
      title.append(tr("Russian - Kuliev & Al-Sa'ady"));
      break;
    case Tafsir::tabary:
      title.append(tr("Al-Tabary"));
      break;
    case Tafsir::sa3dy:
      title.append(tr("Al-Sa'ady"));
      break;
    case Tafsir::tafheem:
      title.append(tr("Tafheem-ul-Quran"));
      break;
    case Tafsir::tanweer:
      title.append(tr("Ibn-Ashoor"));
      break;
    case Tafsir::waseet:
      title.append(tr("Al-Tafsir Al-Waseet"));
      break;
    default:
      title.append(tr("Tafsir"));
      break;
  }

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
    m_fontPrefix + QString::number(m_shownVerse.page).rightJustified(3, '0');

  ui->lbVerseInfo->setText(title);
  ui->lbVerseText->setWordWrap(true);
  ui->lbVerseText->setFont(QFont(fontFamily, m_fontSZ));
  ui->lbVerseText->setText(glyphs);

  QFont sideFont =
    qvariant_cast<QFont>(m_settings->value("Reader/SideContentFont"));
  ui->textEdit->setFont(sideFont);
  ui->textEdit->setHtml(
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
