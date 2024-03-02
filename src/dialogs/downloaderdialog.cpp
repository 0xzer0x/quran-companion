/**
 * @file downloaderdialog.cpp
 * @brief Implementation file for DownloaderDialog
 */

#include "downloaderdialog.h"
#include "ui_downloaderdialog.h"
#include <downloader/contentjob.h>
#include <downloader/qcfjob.h>
#include <downloader/surahjob.h>
#include <utils/stylemanager.h>

DownloaderDialog::DownloaderDialog(QWidget* parent, JobManager* manager)
  : QDialog(parent)
  , ui(new Ui::DownloaderDialog)
  , m_jobMgr(manager)
  , m_config(Configuration::getInstance())
  , m_quranDb(QuranDb::getInstance())
  , m_reciters(Reciter::reciters)
  , m_tafasir(Tafsir::tafasir)
  , m_translations(Translation::translations)

{
  ui->setupUi(this);
  setWindowIcon(
    StyleManager::getInstance().awesome().icon(fa::fa_solid, fa::fa_download));

  // treeview setup
  m_surahDisplayNames = m_quranDb.surahNames();
  QStringList headers;
  headers.append(tr("Name"));
  headers.append(tr("Number"));
  m_treeModel.setHorizontalHeaderLabels(headers);
  ui->treeView->setModel(&m_treeModel);
  populateTreeModel();
  ui->treeView->resizeColumnToContents(0);

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

  connect(m_jobMgr,
          &JobManager::jobCompleted,
          this,
          &DownloaderDialog::downloadCompleted);
  connect(m_jobMgr,
          &JobManager::filesFound,
          this,
          &DownloaderDialog::downloadCompleted);
  connect(m_jobMgr,
          &JobManager::jobAborted,
          this,
          &DownloaderDialog::downloadAborted);
  connect(m_jobMgr,
          &JobManager::jobFailed,
          this,
          &DownloaderDialog::topTaskDownloadError);
  connect(m_jobMgr,
          &JobManager::downloadSpeedUpdated,
          this,
          &DownloaderDialog::updateDownloadSpeed);
}

void
DownloaderDialog::populateTreeModel()
{
  // add reciters
  for (const Reciter& reciter : m_reciters) {
    QStandardItem* item = new QStandardItem(reciter.displayName());
    item->setToolTip(reciter.displayName());
    m_treeModel.invisibleRootItem()->appendRow(item);

    for (int j = 1; j <= 114; j++) {
      QStandardItem* suraItem =
        new QStandardItem(m_surahDisplayNames.at(j - 1));

      QList<QStandardItem*> rw;
      rw.append(suraItem);
      rw.append(new QStandardItem(QString::number(j)));

      item->appendRow(rw);
    }
  }
  // tafsir submenu
  QStandardItem* tafsir =
    new QStandardItem(qApp->translate("SettingsDialog", "Tafsir"));
  tafsir->setData("tafsir", Qt::UserRole);
  m_treeModel.invisibleRootItem()->appendRow(tafsir);
  // -- tafasir
  for (int i = 0; i < m_tafasir.size(); i++) {
    const Tafsir& t = m_tafasir.at(i);
    if (!t.isExtra())
      continue;
    QStandardItem* item = new QStandardItem(t.displayName());
    item->setData("tadb", Qt::UserRole);
    item->setData(i, Qt::UserRole + 1);
    tafsir->appendRow(item);
  }

  // translation submenu
  QStandardItem* translation =
    new QStandardItem(qApp->translate("SettingsDialog", "Translation"));
  tafsir->setData("translation", Qt::UserRole);
  m_treeModel.invisibleRootItem()->appendRow(translation);
  // -- translations
  for (int i = 0; i < m_translations.size(); i++) {
    const Translation& tr = m_translations.at(i);
    if (!tr.isExtra())
      continue;
    QStandardItem* item = new QStandardItem(tr.displayName());
    item->setData("trdb", Qt::UserRole);
    item->setData(i, Qt::UserRole + 1);
    translation->appendRow(item);
  }

  // extras submenu
  QStandardItem* extras = new QStandardItem(tr("Extras"));
  extras->setToolTip(tr("Additional files"));
  extras->setData("extras", Qt::UserRole);
  m_treeModel.invisibleRootItem()->appendRow(extras);
  // -- qcf 2
  QStandardItem* qcf =
    new QStandardItem(qApp->translate("SettingsDialog", "QCF V2"));
  qcf->setData("qcf", Qt::UserRole);
  extras->appendRow(qcf);
}

void
DownloaderDialog::addToDownloading(int reciter, int surah)
{
  // add surah to downloading tasks
  QSet<int>& downloading = m_downloadingSurahs[reciter];
  downloading.insert(surah);
}

void
DownloaderDialog::removeFromDownloading(int reciter, int surah)
{
  QSet<int>& downloading = m_downloadingSurahs[reciter];
  downloading.remove(surah);
  if (downloading.isEmpty())
    m_downloadingSurahs.remove(reciter);
}

void
DownloaderDialog::addToQueue()
{
  static int recitersnum = m_reciters.size();
  QModelIndexList selected = ui->treeView->selectionModel()->selectedRows();

  foreach (const QModelIndex& i, selected) {
    int parent = i.parent().row();
    int current = i.row();
    bool toplevel = parent < 0;

    // recitation (reciter selected)
    if (toplevel && current < recitersnum) {
      for (int surah = 1; surah <= 114; surah++)
        enqueueSurah(current, surah);
    }
    // recitation (surah index selected)
    else if (!toplevel && parent < recitersnum)
      enqueueSurah(parent, current + 1);
    // tafasir
    else if (i.data(Qt::UserRole).toString() == "tadb") {
      QSharedPointer<ContentJob> job = QSharedPointer<ContentJob>::create(
        DownloadJob::TafsirFile, i.data(Qt::UserRole + 1).toInt());
      m_jobMgr->addJob(job);
      addTaskProgress(job);
    }
    // translation
    else if (i.data(Qt::UserRole).toString() == "trdb") {
      QSharedPointer<ContentJob> job = QSharedPointer<ContentJob>::create(
        DownloadJob::TranslationFile, i.data(Qt::UserRole + 1).toInt());
      m_jobMgr->addJob(job);
      addTaskProgress(job);
    }
    // extras
    else if (i.data(Qt::UserRole).toString() == "qcf") {
      QSharedPointer<QcfJob> job = QSharedPointer<QcfJob>::create();
      m_jobMgr->addJob(job);
      addTaskProgress(job);
    }
  }

  setCurrentBar();
  m_jobMgr->start();
}

void
DownloaderDialog::addTaskProgress(QSharedPointer<DownloadJob> job)
{
  int total = job->total();
  QString objName;
  if (job->type() == DownloadJob::Recitation) {
    SurahJob* sJob = qobject_cast<SurahJob*>(job.data());
    QString reciter = m_reciters.at(sJob->reciter()).displayName();
    QString surahName = m_surahDisplayNames.at(sJob->surah() - 1);
    objName = reciter + tr(" // Surah: ") + surahName;
  } else {
    objName = job->name();
  }

  QFrame* prgFrm = new QFrame(ui->scrollAreaWidgetContents);
  prgFrm->setLayout(new QVBoxLayout);
  prgFrm->setObjectName(objName);

  QBoxLayout* downInfo;
  if (m_config.language() == QLocale::Arabic)
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
  downInfo->addStretch(1);
  downInfo->addWidget(downSpeed);
  prgFrm->layout()->addItem(downInfo);

  DownloadProgressBar* dpb =
    new DownloadProgressBar(prgFrm, job->type(), total);
  prgFrm->layout()->addWidget(dpb);
  m_frameLst.append(prgFrm);

  ui->lytFrameView->addWidget(prgFrm);
}

void
DownloaderDialog::enqueueSurah(int reciter, int surah)
{
  bool currentlyDownloading =
    m_downloadingSurahs.value(reciter).contains(surah);
  if (currentlyDownloading)
    return;

  QSharedPointer<SurahJob> sj =
    QSharedPointer<SurahJob>::create(reciter, surah);
  addToDownloading(reciter, surah);
  addTaskProgress(sj);
  m_jobMgr->addJob(sj);
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

  connect(m_jobMgr,
          &JobManager::jobProgressed,
          m_currentBar,
          &DownloadProgressBar::updateProgress);
}

void
DownloaderDialog::updateDownloadSpeed(int value, QString unit)
{
  m_currDownSpeedLb->setText(QString::number(value) + " " + unit + tr("/sec"));
}

void
DownloaderDialog::selectDownload(DownloadJob::Type type, QPair<int, int> info)
{
  QItemSelectionModel* selector = ui->treeView->selectionModel();
  QModelIndex parent;
  QModelIndex task;
  if (type == DownloadJob::Recitation) {
    parent = m_treeModel.index(info.first, 0);
    task = m_treeModel.index(info.second - 1, 0, parent);
  } else if (type == DownloadJob::Qcf) {
    parent = m_treeModel.index(m_treeModel.rowCount() - 1, 0);
    task = m_treeModel.index(0, 0, parent);
  } else if (type == DownloadJob::TafsirFile ||
             type == DownloadJob::TranslationFile) {
    parent = m_treeModel.index(m_treeModel.rowCount() - 2 - !info.first, 0);
    // remove default db indices from current index as defaults are not
    // downloadable
    if (!info.first)
      // tafsir
      info.second -= info.second > 6;
    else
      // translation
      info.second -= 1 + info.second > 5;
    task = m_treeModel.index(info.second, 0, parent);
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
  m_downloadingSurahs.clear();
  m_jobMgr->stop();
  if (!m_finishedFrames.isEmpty()) {
    qDeleteAll(m_finishedFrames);
    m_finishedFrames.clear();
  }
}

void
DownloaderDialog::btnStopClicked()
{
  m_downloadingSurahs.clear();
  m_jobMgr->stop();
}

void
DownloaderDialog::downloadAborted()
{
  m_downloadingSurahs.clear();
  if (!m_frameLst.isEmpty()) {
    qDeleteAll(m_frameLst);
    m_frameLst.clear();
  }
}

void
DownloaderDialog::downloadCompleted(QSharedPointer<DownloadJob> finished)
{
  m_currentBar->setStyling(DownloadProgressBar::completed);
  m_currentLb->setText(m_currentLb->parent()->objectName());
  m_currDownSpeedLb->setText(tr("Download Completed"));
  disconnect(m_jobMgr,
             &JobManager::jobProgressed,
             m_currentBar,
             &DownloadProgressBar::updateProgress);

  if (finished->type() == DownloadJob::Recitation) {
    QSharedPointer<SurahJob> sj = finished.dynamicCast<SurahJob>();
    removeFromDownloading(sj->reciter(), sj->surah());
  }
  if (finished->type() == DownloadJob::TafsirFile ||
      finished->type() == DownloadJob::TranslationFile) {
    m_currentBar->setValue(1);
    m_currentBar->setMaximum(1);
    m_currentBar->setFormat("1 / 1");
  }
  m_finishedFrames.append(m_frameLst.front());
  m_frameLst.pop_front();
  setCurrentBar();
  m_jobMgr->processJobs();
}

void
DownloaderDialog::topTaskDownloadError(QSharedPointer<DownloadJob> failed)
{
  m_currentBar->setStyling(DownloadProgressBar::aborted);
  m_currentLb->setText(m_currentLb->parent()->objectName());
  m_currDownSpeedLb->setText(tr("Download Failed"));
  disconnect(m_jobMgr,
             &JobManager::jobProgressed,
             m_currentBar,
             &DownloadProgressBar::updateProgress);

  if (failed->type() == DownloadJob::Recitation) {
    QSharedPointer<SurahJob> sj = failed.dynamicCast<SurahJob>();
    removeFromDownloading(sj->reciter(), sj->surah());
  }
  m_finishedFrames.append(m_frameLst.front());
  m_frameLst.pop_front();
  setCurrentBar();
  m_jobMgr->processJobs();
}

void
DownloaderDialog::openDownloadsDir()
{
  QUrl url = QUrl::fromLocalFile(
    DirManager::getInstance().downloadsDir().absolutePath());
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
