#include "versedialog.h"
#include "ui_versedialog.h"

VerseDialog::VerseDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::VerseDialog)
{
  ui->setupUi(this);

  QFont sideFont =
    qvariant_cast<QFont>(m_settings->value("Reader/SideContentFont"));
  ui->lbContent->setFont(sideFont);
  ui->lbInfo->setFont(sideFont);
  ui->lbVerse->setFont(QFont("kfgqpc_hafs_uthmanic _script", 20));

  if (m_settings->value("VOTD").toBool())
    showVOTD(true);
}

QString
VerseDialog::votdStringEntry() const
{
  QString entry = QString::number(m_votd.page()).rightJustified(3, '0') + ":" +
                  QString::number(m_votd.surah()).rightJustified(3, '0') + ":" +
                  QString::number(m_votd.number()).rightJustified(3, '0');

  return entry;
}

bool
VerseDialog::votdShown()
{
  QDateTime lastTimestamp;
  if (!m_timestampFile.exists())
    return false;
  if (!m_timestampFile.open(QIODevice::ReadOnly)) {
    qWarning() << "Couldn't open votd.log file for daily notification check";
    return false;
  }
  lastTimestamp =
    QDateTime::fromString(m_timestampFile.readLine().trimmed(), Qt::ISODate);

  m_timestampFile.close();
  return lastTimestamp.daysTo(m_now) <= 0;
}

void
VerseDialog::genVerseOfTheDay()
{
  m_votd = m_quranDb->randomVerse();
}

void
VerseDialog::readVerseOfTheDay()
{
  if (!m_timestampFile.open(QIODevice::ReadOnly)) {
    qWarning() << "Couldn't open votd.log file for daily notification check";
    return;
  }
  m_timestampFile.seek(20);
  QList<QByteArray> data = m_timestampFile.readLine(15).split(':');
  m_votd = Verse{ data.at(0).toInt(), data.at(1).toInt(), data.at(2).toInt() };
  m_timestampFile.close();
  updateLabels();
}

void
VerseDialog::writeTimestamp()
{
  if (!m_timestampFile.open(QIODevice::WriteOnly)) {
    qWarning() << "Couldn't open votd.log file for writing timestamp";
    return;
  }
  m_timestampFile.seek(0);
  m_timestampFile.write(m_now.toString(Qt::ISODate).toLatin1());
  m_timestampFile.write("\n");
  m_timestampFile.write(votdStringEntry().toLatin1());
  m_timestampFile.close();
}

void
VerseDialog::updateLabels()
{
  ui->lbVerse->setText(
    "ﵩ " + m_quranDb->verseText(m_votd.surah(), m_votd.number()) + " ﵨ");
  ui->lbContent->setText(
    m_translationDb->getTranslation(m_votd.surah(), m_votd.number()));
  ui->lbInfo->setText(qApp->translate("BookmarksDialog", "Surah: ") +
                      m_quranDb->surahName(m_votd.surah()) + " - " +
                      qApp->translate("BookmarksDialog", "Verse: ") +
                      QString::number(m_votd.number()));
}

void
VerseDialog::showVOTD(bool startup)
{
  if (votdShown()) {
    if (ui->lbContent->text().isEmpty())
      readVerseOfTheDay();
    if (startup)
      return;
  } else {
    genVerseOfTheDay();
    writeTimestamp();
    updateLabels();
  }

  adjustSize();
  setMinimumSize(size());
  show();
}

void
VerseDialog::mouseReleaseEvent(QMouseEvent* event)
{
  emit navigateToVerse(m_votd);
  this->hide();
}

void
VerseDialog::closeEvent(QCloseEvent* event)
{
  this->hide();
}

VerseDialog::~VerseDialog()
{
  delete ui;
}
