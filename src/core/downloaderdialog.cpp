/**
 * @file downloaderdialog.cpp
 * @brief Implementation file for DownloaderDialog
 */

#include "downloaderdialog.h"
#include "ui_downloaderdialog.h"

DownloaderDialog::DownloaderDialog(QWidget* parent, DownloadManager* downloader)
  : QDialog(parent)
  , ui(new Ui::DownloaderDialog)
  , m_downloaderPtr{ downloader }
  , m_surahDisplayNames{ m_dbMgr->surahNameList() }

{
  ui->setupUi(this);
  setWindowIcon(QIcon(m_resources.filePath("icons/download-manager.png")));

  // treeview setup
  QStringList headers;
  headers.append(tr("Number"));
  headers.append(tr("Name"));
  m_treeModel.setHorizontalHeaderLabels(headers);
  ui->treeView->setModel(&m_treeModel);
  fillTreeView();

  // connectors
  setupConnections();
}

void
DownloaderDialog::setupConnections()
{
  connect(ui->btnAddToQueue,
          &QPushButton::clicked,
          this,
          &DownloaderDialog::addToQueue,
          Qt::UniqueConnection);

  connect(ui->btnDownloads,
          &QPushButton::clicked,
          this,
          &DownloaderDialog::openDownloadsDir,
          Qt::UniqueConnection);

  connect(ui->btnStopQueue,
          &QPushButton::clicked,
          this,
          &DownloaderDialog::btnStopClicked,
          Qt::UniqueConnection);

  connect(ui->btnClearQueue,
          &QPushButton::clicked,
          this,
          &DownloaderDialog::clearQueue,
          Qt::UniqueConnection);

  connect(m_downloaderPtr,
          &DownloadManager::downloadComplete,
          this,
          &DownloaderDialog::surahDownloaded,
          Qt::UniqueConnection);

  connect(m_downloaderPtr,
          &DownloadManager::surahFound,
          this,
          &DownloaderDialog::surahDownloaded,
          Qt::UniqueConnection);

  connect(m_downloaderPtr,
          &DownloadManager::downloadCanceled,
          this,
          &DownloaderDialog::downloadAborted,
          Qt::UniqueConnection);

  connect(m_downloaderPtr,
          &DownloadManager::downloadError,
          this,
          &DownloaderDialog::topTaskDownloadError,
          Qt::UniqueConnection);

  connect(m_downloaderPtr,
          &DownloadManager::downloadSpeedUpdated,
          this,
          &DownloaderDialog::updateDownloadSpeed);
}

void
DownloaderDialog::fillTreeView()
{
  for (const Reciter& reciter : m_recitersList) {
    QStandardItem* item = new QStandardItem(reciter.displayName);
    item->setToolTip(reciter.displayName);

    m_treeModel.invisibleRootItem()->appendRow(item);

    for (int j = 1; j <= 114; j++) {
      QStandardItem* suraItem =
        new QStandardItem(m_surahDisplayNames.at(j - 1));

      QList<QStandardItem*> rw;
      rw.append(new QStandardItem(QString::number(j)));
      rw.append(suraItem);

      item->appendRow(rw);
    }
  }
}

void
DownloaderDialog::addToDownloading(int reciter, int surah)
{
  // add surah to downloading tasks
  QSet<int>& downloading = m_downloadingTasks[reciter];
  downloading.insert(surah);
}

void
DownloaderDialog::removeFromDownloading(int reciter, int surah)
{
  QSet<int>& downloading = m_downloadingTasks[reciter];
  downloading.remove(surah);
  if (downloading.isEmpty())
    m_downloadingTasks.remove(reciter);
}

void
DownloaderDialog::addToQueue()
{
  QModelIndexList selected = ui->treeView->selectionModel()->selectedRows();

  int reciter = -1, surah = -1;
  foreach (const QModelIndex& i, selected) {
    reciter = i.parent().row();
    surah = i.row() + 1;

    if (reciter < 0) {
      reciter = i.row();
      for (surah = 1; surah <= 114; surah++)
        enqueueSurah(reciter, surah);

    } else
      enqueueSurah(reciter, surah);
  }

  setCurrentBar();
  m_downloaderPtr->startQueue();
}

void
DownloaderDialog::addTaskProgress(int reciterIdx, int surah)
{
  QString reciter = m_recitersList.at(reciterIdx).displayName;
  QString surahName = m_surahDisplayNames.at(surah - 1);

  QString objName = reciter + tr(" // Surah: ") + surahName;

  QFrame* prgFrm = new QFrame(ui->scrollAreaWidgetContents);
  prgFrm->setLayout(new QVBoxLayout);
  prgFrm->setObjectName(objName);

  QBoxLayout* downInfo;
  if (m_languageCode == QLocale::Arabic)
    downInfo = new QBoxLayout(QBoxLayout::RightToLeft, prgFrm);
  else
    downInfo = new QHBoxLayout(prgFrm);

  QLabel* lbTitle = new QLabel(prgFrm);
  lbTitle->setObjectName("DownloadInfo");
  lbTitle->setText(prgFrm->objectName());
  QLabel* downSpeed = new QLabel(prgFrm);
  downSpeed->setObjectName("DownloadSpeed");
  downSpeed->setAlignment(Qt::AlignRight);

  downInfo->addWidget(lbTitle);
  downInfo->addWidget(downSpeed);
  prgFrm->layout()->addItem(downInfo);

  DownloadProgressBar* dpb =
    new DownloadProgressBar(prgFrm, m_dbMgr->getSurahVerseCount(surah));
  prgFrm->layout()->addWidget(dpb);
  m_frameLst.append(prgFrm);

  ui->lytFrameView->addWidget(prgFrm);
}

void
DownloaderDialog::enqueueSurah(int reciter, int surah)
{
  bool currentlyDownloading = m_downloadingTasks.value(reciter).contains(surah);
  if (currentlyDownloading)
    return;

  addToDownloading(reciter, surah);
  addTaskProgress(reciter, surah);
  m_downloaderPtr->addSurahToQueue(reciter, surah);
}

void
DownloaderDialog::setCurrentBar()
{
  if (m_frameLst.empty())
    return;

  m_currentLb = m_frameLst.at(0)->findChild<QLabel*>("DownloadInfo");
  m_currDownSpeedLb = m_frameLst.at(0)->findChild<QLabel*>("DownloadSpeed");
  m_currentLb->setText(tr("Downloading: ") +
                       m_currentLb->parent()->objectName());
  m_currentBar = m_frameLst.at(0)->findChild<DownloadProgressBar*>();

  connect(m_downloaderPtr,
          &DownloadManager::downloadProgressed,
          m_currentBar,
          &DownloadProgressBar::updateProgress,
          Qt::UniqueConnection);
}

void
DownloaderDialog::updateDownloadSpeed(int value, QString unit)
{
  m_currDownSpeedLb->setText(QString::number(value) + " " + unit + tr("/sec"));
}

void
DownloaderDialog::selectTask(int reciter, int surah)
{
  QItemSelectionModel* selector = ui->treeView->selectionModel();
  QModelIndex reciterIdx = m_treeModel.index(reciter, 0);
  ui->treeView->collapseAll();
  ui->treeView->expand(reciterIdx);
  selector->clearSelection();
  selector->select(m_treeModel.index(surah - 1, 1, reciterIdx),
                   QItemSelectionModel::Rows | QItemSelectionModel::Select);
}

void
DownloaderDialog::clearQueue()
{
  m_downloadingTasks.clear();
  m_downloaderPtr->stopQueue();
  if (!m_finishedFrames.isEmpty()) {
    qDeleteAll(m_finishedFrames);
    m_finishedFrames.clear();
  }
}

void
DownloaderDialog::btnStopClicked()
{
  m_downloadingTasks.clear();
  m_downloaderPtr->stopQueue();
}

void
DownloaderDialog::surahDownloaded(int reciter, int surah)
{
  m_currentBar->setStyling(DownloadProgressBar::completed);
  m_currentLb->setText(m_currentLb->parent()->objectName());
  m_currDownSpeedLb->setText(tr("Download Completed"));
  disconnect(m_downloaderPtr,
             &DownloadManager::downloadProgressed,
             m_currentBar,
             &DownloadProgressBar::updateProgress);

  removeFromDownloading(reciter, surah);
  m_finishedFrames.append(m_frameLst.front());
  m_frameLst.pop_front();
  setCurrentBar();
}

void
DownloaderDialog::downloadAborted()
{
  m_downloadingTasks.clear();
  if (!m_frameLst.isEmpty()) {
    qDeleteAll(m_frameLst);
    m_frameLst.clear();
  }
}

void
DownloaderDialog::topTaskDownloadError(int reciter, int surah)
{
  m_currentBar->setStyling(DownloadProgressBar::aborted);
  m_currentLb->setText(m_currentLb->parent()->objectName());
  m_currDownSpeedLb->setText(tr("Download Failed"));
  disconnect(m_downloaderPtr,
             &DownloadManager::downloadProgressed,
             m_currentBar,
             &DownloadProgressBar::updateProgress);

  removeFromDownloading(reciter, surah);
  m_finishedFrames.append(m_frameLst.front());
  m_frameLst.pop_front();
  setCurrentBar();
}

void
DownloaderDialog::openDownloadsDir()
{
  QUrl url = QUrl::fromLocalFile(Globals::recitationsDir.absolutePath());
  QDesktopServices::openUrl(url);
}

void
DownloaderDialog::closeEvent(QCloseEvent* event)
{
  this->hide();
}

DownloaderDialog::~DownloaderDialog()
{
  delete ui;
}
