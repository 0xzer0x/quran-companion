/**
 * @file searchdialog.cpp
 * @brief Implementation file for SearchDialog
 */

#include "searchdialog.h"
#include "ui_searchdialog.h"
#include <utils/fontmanager.h>
#include <utils/stylemanager.h>
#include <widgets/clickablelabel.h>

SearchDialog::SearchDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::SearchDialog)
  , m_config(Configuration::getInstance())
  , m_quranDb(QuranDb::getInstance())
  , m_glyphsDb(GlyphsDb::getInstance())
{
  setWindowIcon(StyleManager::getInstance().awesome().icon(
    fa::fa_solid, fa::fa_magnifying_glass));
  ui->setupUi(this);
  ui->frmNavBtns->setLayoutDirection(Qt::LeftToRight);
  ui->btnNext->setIcon(StyleManager::getInstance().awesome().icon(
    fa::fa_solid, fa::fa_arrow_left));
  ui->btnPrev->setIcon(StyleManager::getInstance().awesome().icon(
    fa::fa_solid, fa::fa_arrow_right));
  ui->btnNext->setDisabled(true);
  ui->btnPrev->setDisabled(true);

  ui->btnTransfer->setIcon(StyleManager::getInstance().awesome().icon(
    fa::fa_solid, fa::fa_arrow_right_arrow_left));
  ui->listViewAllSurahs->setModel(&m_modelAllSurahs);
  ui->listViewSelected->setModel(&m_modelSelectedSurahs);
  if (m_config.language() == QLocale::Arabic)
    ui->searchTabWidget->setObjectName("rtlTabWidget");

  fillListView();
  // connectors
  setupConnections();
}

void
SearchDialog::setupConnections()
{
  connect(ui->btnSrch, &QPushButton::clicked, this, &SearchDialog::getResults);
  connect(ui->btnNext, &QPushButton::clicked, this, &SearchDialog::moveFwd);
  connect(ui->btnPrev, &QPushButton::clicked, this, &SearchDialog::moveBwd);
  connect(ui->btnTransfer,
          &QPushButton::clicked,
          this,
          &SearchDialog::btnTransferClicked);
}

void
SearchDialog::getResults()
{
  m_searchText = ui->ledSearchBar->text().trimmed();

  if (!m_lbLst.empty()) {
    qDeleteAll(m_lbLst);
    m_lbLst.clear();
    m_currResults.clear();
  }

  if (m_searchText.isEmpty()) {
    ui->lbResultCount->setText("");
    ui->btnNext->setDisabled(true);
    ui->btnPrev->setDisabled(true);
    return;
  }

  if (!ui->chkSurahsOnly->isChecked()) {
    int range[2];
    range[0] = ui->spnStartPage->value();
    if (ui->spnEndPage->value() < range[0])
      ui->spnEndPage->setValue(range[0]);
    range[1] = ui->spnEndPage->value();

    m_currResults = Verse::fromList(m_quranDb.searchVerses(
      m_searchText, range, ui->chkWholeWord->isChecked()));
  } else {
    m_currResults =
      Verse::fromList(m_quranDb.searchSurahs(m_searchText,
                                             m_selectedSurahMap.values(),
                                             ui->chkWholeWord->isChecked()));
  }
  ui->lbResultCount->setText(QString::number(m_currResults.size()) +
                             tr(" Search results"));
  m_startResult = 0;
  showResults();
}

void
SearchDialog::verseClicked()
{
  QStringList data = sender()->objectName().split('-');
  Verse selected(data.at(0).toInt(), data.at(1).toInt(), data.at(2).toInt());
  emit navigateToVerse(selected);
}

void
SearchDialog::showResults()
{
  int endIdx = m_currResults.size() > m_startResult + 25 ? m_startResult + 25
                                                         : m_currResults.size();

  if (m_startResult == 0)
    ui->btnPrev->setDisabled(true);
  else
    ui->btnPrev->setDisabled(false);
  if (endIdx == m_currResults.size())
    ui->btnNext->setDisabled(true);
  else
    ui->btnNext->setDisabled(false);

  for (int i = m_startResult; i < endIdx; i++) {
    Verse v = m_currResults.at(i);
    QString fontName =
      FontManager::getInstance().verseFontname(m_config.verseType(), v.page());

    VerseFrame* vFrame = new VerseFrame(ui->srclResults);
    QLabel* lbInfo = new QLabel(vFrame);
    ClickableLabel* clkLb = new ClickableLabel(vFrame);

    QString info = tr("Surah: ") + m_quranDb.surahNames().at(v.surah() - 1) +
                   " - " + tr("Verse: ") + QString::number(v.number());
    QString glyphs = m_config.verseType() == Configuration::Qcf
                       ? m_glyphsDb.getVerseGlyphs(v.surah(), v.number())
                       : m_quranDb.verseText(v.surah(), v.number());

    lbInfo->setText(info);
    lbInfo->setMaximumHeight(50);
    lbInfo->setAlignment(Qt::AlignLeft);
    lbInfo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    clkLb->setMargin(5);
    clkLb->setObjectName(QString::number(v.page()) + '-' +
                         QString::number(v.surah()) + '-' +
                         QString::number(v.number()));
    clkLb->setFont(QFont(fontName, 15));
    clkLb->setText(glyphs);
    clkLb->setAlignment(Qt::AlignLeft);
    clkLb->setWordWrap(true);

    connect(clkLb, &ClickableLabel::clicked, this, &SearchDialog::verseClicked);

    vFrame->layout()->addWidget(lbInfo);
    vFrame->layout()->addWidget(clkLb);
    vFrame->setObjectName("F-" + clkLb->objectName());
    ui->srclResults->layout()->addWidget(vFrame);
    m_lbLst.append(vFrame);
  }

  ui->scrollArea->verticalScrollBar()->setValue(0);
}

void
SearchDialog::moveFwd()
{
  if (!m_currResults.empty() && m_startResult + 25 < m_currResults.size()) {
    if (!m_lbLst.empty()) {
      qDeleteAll(m_lbLst);
      m_lbLst.clear();
    }

    m_startResult += 25;
    showResults();
  }
}

void
SearchDialog::moveBwd()
{
  if (!m_currResults.empty() && m_startResult > 0) {
    if (!m_lbLst.empty()) {
      qDeleteAll(m_lbLst);
      m_lbLst.clear();
    }

    m_startResult -= 25;
    showResults();
  }
}

void
SearchDialog::fillListView()
{
  for (int i = 1; i <= 114; i++) {
    QStandardItem* surah = new QStandardItem(m_quranDb.surahNames().at(i - 1));
    m_modelAllSurahs.invisibleRootItem()->appendRow(surah);
  }
}

void
SearchDialog::btnTransferClicked()
{
  QModelIndexList selectedAdd =
    ui->listViewAllSurahs->selectionModel()->selectedRows();
  QModelIndexList selectedRem =
    ui->listViewSelected->selectionModel()->selectedRows();
  QList<QString> removed;

  foreach (const QModelIndex& midx, selectedAdd) {
    if (m_selectedSurahMap.contains(midx.data().toString()))
      continue;
    // KEY: visible surah name - VALUE: surah number
    int sIdx = m_quranDb.surahNames().indexOf(midx.data().toString());
    m_selectedSurahMap.insert(midx.data().toString(), sIdx + 1);
    m_modelSelectedSurahs.appendRow(
      new QStandardItem(midx.data().toString())); // add surah to selected view
  }

  foreach (const QModelIndex& remIdx, selectedRem) {
    // remove from selected map & view
    m_selectedSurahMap.remove(remIdx.data().toString());
    removed.append(remIdx.data().toString());
  }

  foreach (const QString& rem, removed) {
    int r = m_modelSelectedSurahs.findItems(rem).at(0)->row();
    m_modelSelectedSurahs.removeRow(r);
  }
}

void
SearchDialog::closeEvent(QCloseEvent* event)
{
  if (!m_lbLst.empty()) {
    qDeleteAll(m_lbLst);
    m_lbLst.clear();
    ui->lbResultCount->setText("");
    ui->ledSearchBar->clear();
    m_currResults.clear();
    ui->btnNext->setDisabled(true);
    ui->btnPrev->setDisabled(true);
  }

  this->hide();
}

SearchDialog::~SearchDialog()
{
  delete ui;
}
