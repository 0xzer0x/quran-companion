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
  setWindowIcon(Globals::awesome->icon(fa::fa_solid, fa::fa_download));

  // treeview setup
  QStringList headers;
  headers.append(tr("Number"));
  headers.append(tr("Name"));
  m_treeModel.setHorizontalHeaderLabels(headers);
  ui->treeView->setModel(&m_treeModel);
  addRecitationsToModel();

  // connectors
  setupConnections();
}

void
DownloaderDialog::setupConnections()
{
  connect(ui->btnAddToQueue,
          &QPushButton::clicked,
          this,
          &DownloaderDialog::addToQueue);

  connect(ui->btnDownloads,
          &QPushButton::clicked,
          this,
          &DownloaderDialog::openDownloadsDir);

  connect(ui->btnStopQueue,
          &QPushButton::clicked,
          this,
          &DownloaderDialog::btnStopClicked);

  connect(ui->btnClearQueue,
          &QPushButton::clicked,
          this,
          &DownloaderDialog::clearQueue);

  connect(m_downloaderPtr,
          &DownloadManager::downloadCompleted,
          this,
          &DownloaderDialog::surahDownloaded);

  connect(m_downloaderPtr,
          &DownloadManager::filesFound,
          this,
          &DownloaderDialog::surahDownloaded);

  connect(m_downloaderPtr,
          &DownloadManager::downloadCanceled,
          this,
          &DownloaderDialog::downloadAborted);

  connect(m_downloaderPtr,
          &DownloadManager::downloadErrored,
          this,
          &DownloaderDialog::topTaskDownloadError);

  connect(m_downloaderPtr,
          &DownloadManager::downloadSpeedUpdated,
          this,
          &DownloaderDialog::updateDownloadSpeed);
}

void
DownloaderDialog::addRecitationsToModel()
{
  // add reciters
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

  // add extras
  QStandardItem* extras = new QStandardItem(tr("Extras"));
  extras->setToolTip(tr("Additional files"));
  extras->setData("extras", Qt::UserRole);
  m_treeModel.invisibleRootItem()->appendRow(extras);
  // -- qcf 2
  QStandardItem* qcf = new QStandardItem(tr("QCF V2"));
  qcf->setData("qcf", Qt::UserRole);
  extras->appendRow(qcf);
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
  static int extrasRow =
    m_treeModel.rowCount(m_treeModel.invisibleRootItem()->index()) - 1;
  QModelIndexList selected = ui->treeView->selectionModel()->selectedRows();

  foreach (const QModelIndex& i, selected) {
    int parent = i.parent().row();
    int current = i.row();
    bool toplevel = parent < 0;

    // recitation (reciter selected)
    if (toplevel && current < extrasRow) {
      for (int surah = 1; surah <= 114; surah++)
        enqueueSurah(current, surah);
    }
    // recitation (surah index selected)
    else if (!toplevel && parent < extrasRow)
      enqueueSurah(parent, current + 1);
    // extras
    else if (i.data(Qt::UserRole).toString() == "qcf") {
      m_downloaderPtr->addQCFToQueue();
      addTaskProgress(QCF);
    }
  }

  setCurrentBar();
  m_downloaderPtr->startQueue();
}

void
DownloaderDialog::addTaskProgress(DownloadType type, QPair<int, int> info)
{
  QString objName;
  if (type == Recitation) {
    QString reciter = m_recitersList.at(info.first).displayName;
    QString surahName = m_surahDisplayNames.at(info.second - 1);
    objName = reciter + tr(" // Surah: ") + surahName;
  } else {
    objName = tr("QCF V2");
  }

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

  int cnt = type == QCF ? 604 : m_dbMgr->getSurahVerseCount(info.second);
  DownloadProgressBar* dpb = new DownloadProgressBar(prgFrm, cnt);
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
  addTaskProgress(Recitation, QPair<int, int>(reciter, surah));
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
          &DownloadProgressBar::updateProgress);
}

void
DownloaderDialog::updateDownloadSpeed(int value, QString unit)
{
  m_currDownSpeedLb->setText(QString::number(value) + " " + unit + tr("/sec"));
}

void
DownloaderDialog::selectDownload(DownloadType type, QPair<int, int> info)
{
  QItemSelectionModel* selector = ui->treeView->selectionModel();
  QModelIndex parent;
  QModelIndex task;
  if (type == Recitation) {
    parent = m_treeModel.index(info.first, 0);
    task = m_treeModel.index(info.second - 1, 1, parent);
  } else if (type == QCF) {
    parent = m_treeModel.index(m_treeModel.rowCount() - 1, 0);
    task = m_treeModel.index(0, 0, parent);
  }

  ui->treeView->collapseAll();
  ui->treeView->expand(parent);
  selector->clearSelection();
  selector->select(task,
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
DownloaderDialog::surahDownloaded(DownloadType type, const QList<int>& metainfo)
{
  m_currentBar->setStyling(DownloadProgressBar::completed);
  m_currentLb->setText(m_currentLb->parent()->objectName());
  m_currDownSpeedLb->setText(tr("Download Completed"));
  disconnect(m_downloaderPtr,
             &DownloadManager::downloadProgressed,
             m_currentBar,
             &DownloadProgressBar::updateProgress);

  if (type == Recitation)
    removeFromDownloading(metainfo[0], metainfo[1]);
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
DownloaderDialog::topTaskDownloadError(DownloadType type,
                                       const QList<int>& metainfo)
{
  m_currentBar->setStyling(DownloadProgressBar::aborted);
  m_currentLb->setText(m_currentLb->parent()->objectName());
  m_currDownSpeedLb->setText(tr("Download Failed"));
  disconnect(m_downloaderPtr,
             &DownloadManager::downloadProgressed,
             m_currentBar,
             &DownloadProgressBar::updateProgress);

  if (type == Recitation)
    removeFromDownloading(metainfo[0], metainfo[1]);
  m_finishedFrames.append(m_frameLst.front());
  m_frameLst.pop_front();
  setCurrentBar();
}

void
DownloaderDialog::openDownloadsDir()
{
  QUrl url = QUrl::fromLocalFile(Globals::downloadsDir.absolutePath());
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
