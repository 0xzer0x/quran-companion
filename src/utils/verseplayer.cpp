#include "verseplayer.h"

/*!
 * \brief VersePlayer::VersePlayer class constructor
 * \param parent QObject for pointer memory management
 * \param dbPtr pointer to the DBManager API to query data from different database files
 * \param initSurah surah to start recitation with
 * \param initVerse verse in the inital surah to start with
 */
VersePlayer::VersePlayer(QObject *parent, DBManager *dbPtr, Verse initVerse, int reciterIdx)
    : QMediaPlayer(parent)
    , m_output{new QAudioOutput()}
    , m_activeVerse{initVerse}
    , m_reciter{reciterIdx}
{
    m_reciterDir.setPath(QApplication::applicationDirPath());
    m_reciterDir.cd("audio");

    m_dbPtr = dbPtr;
    updateSurahVerseCount();
    setAudioOutput(m_output);
    fillRecitersList();

    // Connectors
    connect(this,
            &QMediaPlayer::mediaStatusChanged,
            this,
            &VersePlayer::verseStateChanged,
            Qt::UniqueConnection);

    connect(this, &VersePlayer::newVerse, this, &VersePlayer::playCurrentVerse, Qt::UniqueConnection);

    m_reciterDir.cd(m_recitersList.at(m_reciter).baseDirName);
    setVerseFile(constructVerseFilename());
}

void VersePlayer::fillRecitersList()
{
    Reciter husary{"Al-Husary",
                   tr("Al-Husary"),
                   m_reciterDir.filePath("bismillah/husary.mp3"),
                   "https://everyayah.com/data/Husary_64kbps/"};

    Reciter husaryQasr{
        "Al-Husary_(Qasr)",
        tr("Al-Husary (Qasr)"),
        husary.basmallahPath,
        "https://github.com/0xzer0x/quran-companion/raw/audio-files/audio/husary_qasr_64kbps/"};

    Reciter husaryMujawwad{"Al-Husary_(Mujawwad)",
                           tr("Al-Husary (Mujawwad)"),
                           husary.basmallahPath,
                           "https://everyayah.com/data/Husary_Mujawwad_64kbps/"};

    Reciter abdulbasit{"Abdul-Basit",
                       tr("Abdul-Basit"),
                       m_reciterDir.filePath("bismillah/abdul-basit.mp3"),
                       "https://everyayah.com/data/Abdul_Basit_Murattal_64kbps/"};

    Reciter abdulbaitMujawwad{"Abdul-Basit_(Mujawwad)",
                              tr("Abdul-Basit (Mujawwad)"),
                              abdulbasit.basmallahPath,
                              "https://everyayah.com/data/Abdul_Basit_Mujawwad_128kbps/"};

    Reciter menshawi{"Menshawi",
                     tr("Menshawi"),
                     m_reciterDir.filePath("bismillah/menshawi.mp3"),
                     "https://everyayah.com/data/Minshawy_Murattal_128kbps/"};

    Reciter menshawiMujawwad{"Menshawi_(Mujawwad)",
                             tr("Menshawi (Mujawwad)"),
                             menshawi.basmallahPath,
                             "https://everyayah.com/data/Minshawy_Mujawwad_64kbps/"};

    Reciter alafasy{"Mishary_Alafasy",
                    tr("Mishary Alafasy"),
                    m_reciterDir.filePath("bismillah/alafasy.mp3"),
                    "https://everyayah.com/data/Alafasy_64kbps/"};

    Reciter tunaiji{"Khalefa_Al-Tunaiji",
                    tr("Khalefa Al-Tunaiji"),
                    m_reciterDir.filePath("bismillah/tunaiji.mp3"),
                    "https://everyayah.com/data/khalefa_al_tunaiji_64kbps/"};

    Reciter dussary{"Yasser_Ad-Dussary",
                    tr("Yasser Ad-Dussary"),
                    m_reciterDir.filePath("bismillah/ad-dussary.mp3"),
                    "https://everyayah.com/data/Yasser_Ad-Dussary_128kbps/"};

    Reciter banna{"Mahmoud_Al-Banna",
                  tr("Mahmoud Al-Banna"),
                  m_reciterDir.filePath("bismillah/al-banna.mp3"),
                  "https://everyayah.com/data/mahmoud_ali_al_banna_32kbps/"};

    m_recitersList.append(husary);
    m_recitersList.append(husaryQasr);
    m_recitersList.append(husaryMujawwad);
    m_recitersList.append(abdulbasit);
    m_recitersList.append(abdulbaitMujawwad);
    m_recitersList.append(menshawi);
    m_recitersList.append(menshawiMujawwad);
    m_recitersList.append(alafasy);
    m_recitersList.append(tunaiji);
    m_recitersList.append(dussary);
    m_recitersList.append(banna);
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

QList<Reciter> VersePlayer::recitersList() const
{
    return m_recitersList;
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
    m_activeVerse.number = 0;

    // no basmalah with surah al-tawbah (9)
    if (m_activeVerse.surah == 9) {
        nextVerse();
    } else {
        if (m_activeVerse.surah == 1)
            m_activeVerse.number = 1;

        setSource(QUrl::fromLocalFile(m_recitersList.at(m_reciter).basmallahPath));
        play();
    }
}

/* -------------------- Setters ----------------------- */

bool VersePlayer::changeReciter(int reciterIdx)
{
    if (m_activeVerse.number == 0)
        m_activeVerse.number = 1;

    stop();
    if (reciterIdx != m_reciter) {
        m_reciterDir.cdUp();
        m_reciterDir.cd(m_recitersList.at(reciterIdx).baseDirName);
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
        qDebug() << "Emitting missing signal...";
        emit missingVerseFile();
        return false;
    }

    m_verseFile = newVerseFilename;
    setSource(QUrl::fromLocalFile(m_reciterDir.filePath(m_verseFile)));

    return true;
}

void VersePlayer::updateSurahVerseCount()
{
    m_surahCount = m_dbPtr->getSurahVerseCount(m_activeVerse.surah);
}

/* -------------------- Getters ----------------------- */

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
