/**
 * @file bookmarksdialog.cpp
 * @brief Implementation for BookmarksDialog
 */

#include "bookmarksdialog.h"
#include "ui_bookmarksdialog.h"
#include <set>
#include <utils/fontmanager.h>
#include <service/servicefactory.h>
#include <utils/stylemanager.h>

BookmarksDialog::BookmarksDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::BookmarksDialog)
  , m_config(Configuration::getInstance())
  , m_navigator(Navigator::getInstance())
  , m_quranService(ServiceFactory::quranService())
  , m_bookmarkService(ServiceFactory::bookmarkService())
  , m_glyphService(ServiceFactory::glyphService())
{
  ui->setupUi(this);
  ui->navBar->setLayoutDirection(Qt::LeftToRight);
  ui->btnNext->setIcon(StyleManager::getInstance().awesome().icon(
    fa::fa_solid, fa::fa_arrow_left));
  ui->btnPrev->setIcon(StyleManager::getInstance().awesome().icon(
    fa::fa_solid, fa::fa_arrow_right));
  ui->scrollArea->setLayoutDirection(Qt::LeftToRight);
  setWindowIcon(
    StyleManager::getInstance().awesome().icon(fa::fa_solid, fa::fa_bookmark));
  ui->listViewBookmarkedSurahs->setModel(&m_surahsModel);

  loadDefault();
  setupConnections();
}

void
BookmarksDialog::setupConnections()
{
  connect(ui->listViewBookmarkedSurahs,
          &QListView::clicked,
          this,
          &BookmarksDialog::surahSelected);
  connect(
    ui->btnNext, &QPushButton::clicked, this, &BookmarksDialog::btnNextClicked);
  connect(
    ui->btnPrev, &QPushButton::clicked, this, &BookmarksDialog::btnPrevClicked);
}

void
BookmarksDialog::addEmptyBookmarksLabel()
{
  QLabel* empty = new QLabel(this);
  QFont fnt = empty->font();
  fnt.setPointSize(14);
  empty->setFont(fnt);
  empty->setText(
    tr("No bookmarks available. Start bookmarking verses to see them here."));
  empty->setAlignment(Qt::AlignCenter);
  ui->layoutFavorites->addWidget(empty);
  m_frames.append(empty);
}

void
BookmarksDialog::loadDefault()
{
  m_shownSurah = 0;
  loadBookmarks();
  loadSurahs();
  ui->listViewBookmarkedSurahs->selectionModel()->select(
    m_surahsModel.index(0, 0),
    QItemSelectionModel::SelectionFlag::Rows | QItemSelectionModel::Select);
}

void
BookmarksDialog::showWindow()
{
  if (!m_frames.empty()) {
    qDeleteAll(m_frames);
    m_frames.clear();
  }

  loadDefault();
  this->show();
}

void
BookmarksDialog::loadBookmarks(int surah)
{
  if (m_shownSurah != surah) {
    m_shownSurah = surah;
    m_shownVerses = m_bookmarkService->bookmarkedVerses(surah);
    if (m_shownSurah == -1)
      m_allBookmarked = m_shownVerses;
  }

  int end = m_startIdx + 10 > m_shownVerses.size() ? m_shownVerses.size()
                                                   : m_startIdx + 10;

  if (m_startIdx == 0)
    ui->btnPrev->setDisabled(true);
  else
    ui->btnPrev->setDisabled(false);
  if (end == m_shownVerses.size())
    ui->btnNext->setDisabled(true);
  else
    ui->btnNext->setDisabled(false);

  if (end == 0) {
    addEmptyBookmarksLabel();
    return;
  }

  for (int i = m_startIdx; i < end; i++) {
    const Verse& verse = m_shownVerses.at(i);
    QString fontName = FontManager::getInstance().verseFontname(
      m_config.verseType(), verse.page());

    QFrame* frame = new QFrame(ui->scrlBookmarks);
    frame->setProperty("bookmark", true);
    QHBoxLayout* frmLayout = new QHBoxLayout();
    QVBoxLayout* lbLayout = new QVBoxLayout();
    QVBoxLayout* btnLayout = new QVBoxLayout();
    QLabel* lbMeta = new QLabel(frame);
    QLabel* verseLb = new QLabel(frame);
    QPushButton* goToVerse = new QPushButton(tr("Go to verse"), frame);
    QPushButton* removeFromFav = new QPushButton(tr("Remove"), frame);
    goToVerse->setCursor(Qt::PointingHandCursor);
    removeFromFav->setCursor(Qt::PointingHandCursor);
    goToVerse->setStyleSheet(
      "QPushButton { min-width: 150px; max-width: 150px; }");
    removeFromFav->setStyleSheet(
      "QPushButton { min-width: 150px; max-width: 150px; }");

    connect(
      goToVerse, &QPushButton::clicked, this, &BookmarksDialog::btnGoToVerse);
    connect(
      removeFromFav, &QPushButton::clicked, this, &BookmarksDialog::btnRemove);

    QString info = tr("Surah: ") +
                   m_quranService->surahNames().at(verse.surah() - 1) + " - " +
                   tr("Verse: ") + QString::number(verse.number());
    QString glyphs =
      m_config.verseType() == Configuration::Qcf
        ? m_glyphService->getVerseGlyphs(verse.surah(), verse.number())
        : m_quranService->verseText(verse.surah(), verse.number());

    lbMeta->setText(info);
    lbMeta->setAlignment(Qt::AlignLeft);

    verseLb->setFont(QFont(fontName, 15));
    verseLb->setText(glyphs);
    verseLb->setAlignment(Qt::AlignLeft);
    verseLb->setWordWrap(true);
    verseLb->setMargin(5);

    lbLayout->addStretch();
    lbLayout->addWidget(lbMeta);
    lbLayout->addWidget(verseLb);
    lbLayout->addStretch();

    btnLayout->setSpacing(2);
    btnLayout->addStretch();
    btnLayout->addWidget(goToVerse);
    btnLayout->addWidget(removeFromFav);
    btnLayout->addStretch();

    frmLayout->addItem(btnLayout);
    frmLayout->addItem(lbLayout);

    frame->setLayout(frmLayout);
    frame->setObjectName(QString::number(verse.page()) + '-' +
                         QString::number(verse.surah()) + '-' +
                         QString::number(verse.number()));

    ui->layoutFavorites->addWidget(frame);
    m_frames.append(frame);
  }
}

void
BookmarksDialog::loadSurahs()
{
  qDeleteAll(m_surahsModel.children());
  m_surahsModel.clear();
  QStandardItem* item = new QStandardItem(tr("All"));
  item->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
  item->setSizeHint(QSize(70, 40));
  m_surahsModel.appendRow(item);

  std::set<int> surahs;
  foreach (const Verse& v, m_allBookmarked) {
    surahs.insert(v.surah());
  }

  for (int s : surahs) {
    item = new QStandardItem(m_quranService->surahNames().at(s - 1));
    item->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
    item->setToolTip(item->text());
    item->setData(s, Qt::UserRole);
    item->setSizeHint(QSize(70, 40));
    m_surahsModel.appendRow(item);
  }
}

void
BookmarksDialog::surahSelected(const QModelIndex& index)
{
  if (!m_frames.empty()) {
    qDeleteAll(m_frames);
    m_frames.clear();
    m_startIdx = 0;
  }

  int idx = index.data(Qt::UserRole).toInt();
  if (idx == 0) {
    loadBookmarks();
  } else {
    loadBookmarks(idx);
  }
}

void
BookmarksDialog::btnGoToVerse()
{
  QStringList info = sender()->parent()->objectName().split('-');
  Verse verse(info.at(0).toInt(), info.at(1).toInt(), info.at(2).toInt());
  m_navigator.navigateToVerse(verse);
}

void
BookmarksDialog::btnRemove()
{
  QStringList info = sender()->parent()->objectName().split('-');
  Verse verse{ info.at(0).toInt(), info.at(1).toInt(), info.at(2).toInt() };

  if (m_bookmarkService->removeBookmark(verse, true)) {
    QFrame* frm = qobject_cast<QFrame*>(sender()->parent());
    int idx = m_frames.indexOf(frm);
    if (idx != -1)
      m_frames.remove(idx);
    delete frm;

    // no more bookmarks in this surah
    if (m_frames.isEmpty())
      loadDefault();
  }
}

void
BookmarksDialog::btnNextClicked()
{
  if (!m_shownVerses.empty() && m_startIdx + 10 < m_shownVerses.size()) {
    if (!m_frames.empty()) {
      qDeleteAll(m_frames);
      m_frames.clear();
    }
    m_startIdx += 10;
    loadBookmarks(m_shownSurah);
  }
}

void
BookmarksDialog::btnPrevClicked()
{
  if (!m_shownVerses.empty() && m_startIdx > 0) {
    if (!m_frames.empty()) {
      qDeleteAll(m_frames);
      m_frames.clear();
    }
    m_startIdx -= 10;
    loadBookmarks(m_shownSurah);
  }
}

void
BookmarksDialog::closeEvent(QCloseEvent* event)
{
  this->hide();
}

BookmarksDialog::~BookmarksDialog()
{
  delete ui;
}
