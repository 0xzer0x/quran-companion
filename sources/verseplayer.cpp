#include "../headers/verseplayer.h"

/*!
 * \brief VersePlayer::VersePlayer class constructor
 * \param parent QObject for pointer memory management
 * \param dbPtr pointer to the DBManager API to query data from different database files
 * \param initSurah surah to start recitation with
 * \param initVerse verse in the inital surah to start with
 */
VersePlayer::VersePlayer(QObject *parent, DBManager *dbPtr, int initSurah, int initVerse)
    : QMediaPlayer{parent}
    , m_output{new QAudioOutput()}
{
    m_reciterDir.setPath(QApplication::applicationDirPath());
    m_reciterDir.cd("audio");
    m_reciterDir.cd("Al-Husary");

    m_dbPtr = dbPtr;
    m_surahIdx = initSurah;
    m_verseNum = initVerse;
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

    m_reciterDirNames.insert(0, "Al-Husary");
    m_reciterDirNames.insert(1, "Al-Husary_(Qasr)");
    m_reciterDirNames.insert(2, "Abdul-Basit");
    m_reciterDirNames.insert(3, "Menshawi");
    m_reciterDirNames.insert(4, "Mishary_Alafasy");

    m_bsmlPaths.append("qrc:/assets/bismillah/husary.mp3");
    m_bsmlPaths.append("qrc:/assets/bismillah/husary.mp3");
    m_bsmlPaths.append("qrc:/assets/bismillah/abdul-basit.mp3");
    m_bsmlPaths.append("qrc:/assets/bismillah/menshawi.mp3");
    m_bsmlPaths.append("qrc:/assets/bismillah/alafasy.mp3");
}

/*!
 * \brief VersePlayer::nextVerse method to increment the verse & surah variables appropriately according
 * to the surah. emits a signal on verse change & on changing from surah to another
 */
void VersePlayer::nextVerse()
{
    // if last verse in surah, play basmalah before playing first verse
    if (m_verseNum == m_surahCount) {
        // if last verse in surah an-nas (114), do nothing (i.e stop playback)
        if ((m_surahIdx < 114)) {
            m_surahIdx++;
            m_verseNum = 0;
            updateSurahVerseCount(); // set new surah verse count
            emit newSurah();         // signals surah change
        }

    } else {
        m_verseNum++;
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
    filename.append(QString::number(m_surahIdx).rightJustified(3, '0'));
    filename.append(QString::number(m_verseNum).rightJustified(3, '0'));

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
    if (m_surahIdx == 9 || m_surahIdx == 1) {
        nextVerse();
    } else {
        qInfo() << m_bsmlPaths.at(m_reciter);
        setSource(QUrl::fromLocalFile(m_bsmlPaths.at(m_reciter)));
        play();
    }

    // Debugging
    qInfo() << "SurahIdx:" << m_surahIdx << "Count:" << m_surahCount;
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

void VersePlayer::setSurahIdx(int newSurahIdx)
{
  m_surahIdx = newSurahIdx;
}

void VersePlayer::setVerseNum(int newVerseNum)
{
  m_verseNum = newVerseNum;
}

void VersePlayer::updateSurahVerseCount()
{
  m_surahCount = m_dbPtr->getSurahVerseCount(m_surahIdx);
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
  return m_surahIdx;
}

int VersePlayer::surahCount() const
{
  return m_surahCount;
}

int VersePlayer::verseNum() const
{
  return m_verseNum;
}
