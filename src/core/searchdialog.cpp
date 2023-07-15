#include "searchdialog.h"
#include "ui_searchdialog.h"

/*!
 * \brief SearchDialog::SearchDialog class constructor
 *
 * @param parent pointer to parent widget
 * @param dbPtr pointer to database management interface
 */
SearchDialog::SearchDialog(QWidget* parent, DBManager* dbPtr)
  : QDialog(parent)
  , ui(new Ui::SearchDialog)
  , m_dbMgr{ dbPtr }
  , m_surahNames{ m_dbMgr->surahNameList() }
{
  setWindowIcon(QIcon(m_resources.filePath("icons/search.png")));
  ui->setupUi(this);
  ui->frmNavBtns->setLayoutDirection(Qt::LeftToRight);
  ui->btnNext->setDisabled(true);
  ui->btnPrev->setDisabled(true);

  ui->listViewAllSurahs->setModel(&m_modelAllSurahs);
  ui->listViewSelected->setModel(&m_modelSelectedSurahs);
  fillListView();

  // connectors
  setupConnections();
}

void
SearchDialog::setupConnections()
{
  QShortcut* ctrlQ = new QShortcut(QKeySequence("Ctrl+Q"), this);
  connect(ctrlQ, &QShortcut::activated, this, &SearchDialog::close);
  connect(ui->btnSrch,
          &QPushButton::clicked,
          this,
          &SearchDialog::getResults,
          Qt::UniqueConnection);
  connect(ui->btnNext,
          &QPushButton::clicked,
          this,
          &SearchDialog::moveFwd,
          Qt::UniqueConnection);
  connect(ui->btnPrev,
          &QPushButton::clicked,
          this,
          &SearchDialog::moveBwd,
          Qt::UniqueConnection);
  connect(ui->btnTransfer,
          &QPushButton::clicked,
          this,
          &SearchDialog::btnTransferClicked,
          Qt::UniqueConnection);
}

/*!
 * \brief SearchDialog::getResults slot to get search results and update UI
 * accordingly
 */
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

    m_currResults =
      m_dbMgr->searchVerses(m_searchText, range, ui->chkWholeWord->isChecked());
  } else {
    m_currResults = m_dbMgr->searchSurahs(
      m_searchText, m_selectedSurahMap.values(), ui->chkWholeWord->isChecked());
  }
  ui->lbResultCount->setText(QString::number(m_currResults.size()) +
                             tr(" Search results"));
  m_startResult = 0;
  showResults();
}

/*!
 * \brief SearchDialog::verseClicked slot to emit signal for mainwindow to
 * navigate to the clicked verse
 */
void
SearchDialog::verseClicked()
{
  QStringList data = sender()->objectName().split('-');
  Verse selected{ data.at(0).toInt(), data.at(1).toInt(), data.at(2).toInt() };
  emit navigateToVerse(selected);
}

/*!
 * \brief SearchDialog::showResults slot to display 50 search results according
 * to the startIdx
 */
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
      m_fontPrefix + QString::number(v.page).rightJustified(3, '0');
    HighlightFrame* vFrame = new HighlightFrame(ui->srclResults);
    QLabel* lbInfo = new QLabel(vFrame);
    ClickableLabel* clkLb = new ClickableLabel(vFrame);

    QString info = tr("Surah: ") + m_surahNames.at(v.surah - 1) + " - " +
                   tr("Verse: ") + QString::number(v.number);
    lbInfo->setText(info);
    lbInfo->setMaximumHeight(50);
    lbInfo->setAlignment(Qt::AlignLeft);
    lbInfo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    clkLb->setMargin(5);
    clkLb->setObjectName(QString::number(v.page) + '-' +
                         QString::number(v.surah) + '-' +
                         QString::number(v.number));
    clkLb->setFont(QFont(fontName, 15));
    clkLb->setText(m_dbMgr->getVerseGlyphs(v.surah, v.number));
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

/*!
 * \brief SearchDialog::moveFwd slot for displaying the next 50 results from the
 * search results
 */
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

/*!
 * \brief SearchDialog::moveBwd slot for displaying the previous 50 results from
 * the search results
 */
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
    QStandardItem* surah = new QStandardItem(m_surahNames.at(i - 1));
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
    int sIdx = m_surahNames.indexOf(midx.data().toString());
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
