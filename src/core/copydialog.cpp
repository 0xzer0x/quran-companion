#include "copydialog.h"
#include "ui_copydialog.h"

CopyDialog::CopyDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::CopyDialog)
{
  ui->setupUi(this);
  ui->cmbCopyFrom->setValidator(m_verseValidator);
  ui->cmbCopyTo->setValidator(m_verseValidator);
  connect(
    ui->buttonBox, &QDialogButtonBox::accepted, this, &CopyDialog::copyRange);
}

void
CopyDialog::show(const Verse& curr)
{
  ui->cmbCopyFrom->clear();
  ui->cmbCopyTo->clear();
  ui->lbCopySurahName->setText(m_dbMgr->getSurahName(curr.surah));

  m_surah = curr.surah;
  m_surahCnt = m_dbMgr->getSurahVerseCount(curr.surah);
  for (int i = 1; i <= m_surahCnt; i++) {
    ui->cmbCopyFrom->addItem(QString::number(i));
    ui->cmbCopyTo->addItem(QString::number(i));
  }
  int idx = curr.number ? curr.number - 1 : 0;
  ui->cmbCopyFrom->setCurrentIndex(idx);
  ui->cmbCopyTo->setCurrentIndex(idx);
  m_verseValidator->setBottom(1);
  m_verseValidator->setTop(m_surahCnt);

  QDialog::show();
}

void
CopyDialog::copyRange()
{
  int from = ui->cmbCopyFrom->currentText().toInt();
  int to = ui->cmbCopyTo->currentText().toInt();
  if (to < from || from < 0 || from > m_surahCnt || to < 0 || to > m_surahCnt) {
    QMessageBox::warning(
      this, tr("Invalid range"), tr("The entered verse range is invalid"));
    return;
  }

  QString final = "{ ";
  QClipboard* clip = QApplication::clipboard();
  for (int i = from; i <= to; i++) {
    QString text = m_dbMgr->getVerseText(m_surah, i);
    text.remove(text.size() - 1, 1);
    text += "(" + QString::number(i) + ") ";
    final.append(text);
  }

  final += "} [" + m_dbMgr->surahNameList().at(m_surah - 1) + "]";
  clip->setText(final);
}

void
CopyDialog::closeEvent(QCloseEvent* event)
{
  this->hide();
}

CopyDialog::~CopyDialog()
{
  delete ui;
}
