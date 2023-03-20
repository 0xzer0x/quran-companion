#include "verseplayer.h"

/*!
 * \brief VersePlayer::VersePlayer class constructor
 * \param parent QObject for pointer memory management
 * \param dbPtr pointer to the DBManager API to query data from different database files
 * \param initSurah surah to start recitation with
 * \param initVerse verse in the inital surah to start with
 */
VersePlayer::VersePlayer(QObject *parent, DBManager *dbPtr, Verse initVerse)
    : QMediaPlayer{parent}
    , m_output{new QAudioOutput()}
    , m_activeVerse{initVerse}
{
    m_reciterDir.setPath(QApplication::applicationDirPath());
    m_reciterDir.cd("audio");
    m_reciterDir.cd("Al-Husary");

    m_dbPtr = dbPtr;
    updateSurahVerseCount();

    setAudioOutput(m_output);
    setVerseFile(constructVerseFilename());

    // Connectors
    connect(this,
            &QMediaPlayer::mediaStatusChanged,
            this,
            &VersePlayer::verseStateChanged,
            Qt::UniqueConnection);

    connect(this, &VersePlayer::newVerse, this, &VersePlayer::playCurrentVerse, Qt::UniqueConnection);

    m_reciterDirNames.append("Al-Husary");
    m_reciterDirNames.append("Al-Husary_(Qasr)");
    m_reciterDirNames.append("Al-Husary_(Mujawwad)");
    m_reciterDirNames.append("Abdul-Basit");
    m_reciterDirNames.append("Abdul-Basit_(Mujawwad)");
    m_reciterDirNames.append("Menshawi");
    m_reciterDirNames.append("Menshawi_(Mujawwad)");
    m_reciterDirNames.append("Mishary_Alafasy");
    m_reciterDirNames.append("Khalefa_Al-tunaiji");
    m_reciterDirNames.append("Yasser_Ad-dussary");
    m_reciterDirNames.append("Mahmoud_Al-banna");

    m_bsmlPaths.append("qrc:/assets/audio/bismillah/husary.mp3");
    m_bsmlPaths.append("qrc:/assets/audio/bismillah/husary.mp3");
    m_bsmlPaths.append("qrc:/assets/audio/bismillah/abdul-basit.mp3");
    m_bsmlPaths.append("qrc:/assets/audio/bismillah/menshawi.mp3");
    m_bsmlPaths.append("qrc:/assets/audio/bismillah/alafasy.mp3");
}

void VersePlayer::setVerse(Verse &newVerse)
{
    m_activeVerse = newVerse;
}

/*!
 * \brief VersePlayer::nextVerse method to increment the verse & surah variables appropriately according
 * to the surah. emits a signal on verse change & on changing from surah to another
 */
void VersePlayer::nextVerse()
{
    // if last verse in surah, play basmalah before playing first verse
    if (m_activeVerse.number == m_surahCount) {
        // if last verse in surah an-nas (114), do nothing (i.e stop playback)
        if ((m_activeVerse.surah < 114)) {
            m_activeVerse.surah++;
            m_activeVerse.number = 0;
            updateSurahVerseCount(); // set new surah verse count
            emit newSurah();         // signals surah change
        }

    } else {
        m_activeVerse.number++;
        emit newVerse(); // signals active verse change
    }
}

/*!
 * \brief VersePlayer::verseStateChanged slot to call the nextVerse() method on verse audio end
 * \param status status of the media file, refer to QMediaPlayer docs for enum.
 */
void VersePlayer::verseStateChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia)
        nextVerse();
}

void VersePlayer::changeUsedAudioDevice(QAudioDevice dev)
{
    m_output->setDevice(dev);
    setAudioOutput(m_output);
}

QString VersePlayer::constructVerseFilename()
{
    // construct verse mp3 filename e.g. 002005.mp3
    QString filename;
    filename.append(QString::number(m_activeVerse.surah).rightJustified(3, '0'));
    filename.append(QString::number(m_activeVerse.number).rightJustified(3, '0'));

    filename.append(".mp3");
    return filename;
}

void VersePlayer::playCurrentVerse()
{
    QString filename = constructVerseFilename();

    // set the attribute / source for QMediaPlayer
    if (setVerseFile(filename))
        // start playback of audio
        play();
}

void VersePlayer::playBasmalah()
{
    // no basmalah with surah al-tawbah (9)
    if (m_activeVerse.surah == 9 || m_activeVerse.surah == 1) {
        nextVerse();
    } else {
        qInfo() << m_bsmlPaths.at(m_reciter);
        setSource(QUrl::fromLocalFile(m_bsmlPaths.at(m_reciter)));
        play();
    }

    // Debugging
    qInfo() << "SurahIdx:" << m_activeVerse.surah << "Count:" << m_surahCount;
}

/* -------------------- Setters ----------------------- */

bool VersePlayer::changeReciter(int reciterIdx)
{
    stop();
    if (reciterIdx != m_reciter) {
        m_reciterDir.cdUp();
        m_reciterDir.cd(m_reciterDirNames.at(reciterIdx));
        m_reciter = reciterIdx;
    }

    if (hasAudio())
        if (!setVerseFile(constructVerseFilename()))
            setSource(QUrl());

    return true;
}

bool VersePlayer::setVerseFile(const QString &newVerseFilename)
{
    if (!m_reciterDir.exists(newVerseFilename)) {
        qInfo() << "[DEBUG] Emitting missing signal...";
        emit missingVerseFile();
        return false;
    }

    m_verseFile = newVerseFilename;
    qInfo() << m_reciterDir.filePath(m_verseFile);
    setSource(QUrl::fromLocalFile(m_reciterDir.filePath(m_verseFile)));

    return true;
}

void VersePlayer::updateSurahVerseCount()
{
    m_surahCount = m_dbPtr->getSurahVerseCount(m_activeVerse.surah);
}

/* -------------------- Getters ----------------------- */

QList<QString> VersePlayer::reciterDirNames() const
{
  return m_reciterDirNames;
}

QAudioOutput *VersePlayer::getOutput() const
{
  return m_output;
}

QString VersePlayer::verseFilename() const
{
  return m_verseFile;
}

int VersePlayer::surahIdx() const
{
  return m_activeVerse.surah;
}

int VersePlayer::surahCount() const
{
  return m_surahCount;
}

int VersePlayer::verseNum() const
{
  return m_activeVerse.number;
}
