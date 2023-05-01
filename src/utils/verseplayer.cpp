#include "verseplayer.h"

/*!
 * \brief VersePlayer::VersePlayer class constructor
 * \param parent QObject for pointer memory management
 * \param dbPtr pointer to the DBManager API to query data from different
 * database files
 * \param initSurah surah to start recitation with
 * \param initVerse verse in the inital surah to start with
 */
VersePlayer::VersePlayer(QObject* parent,
                         DBManager* dbPtr,
                         Verse initVerse,
                         int reciterIdx)
  : QMediaPlayer(parent)
  , m_activeVerse{ initVerse }
  , m_reciter{ reciterIdx }
  , m_dbMgr{ dbPtr }
{

  updateSurahVerseCount();
  m_output = new QAudioOutput(this);
  setAudioOutput(m_output);
  fillRecitersList();

  m_reciterDir.setPath(QDir::currentPath());
  if (!m_reciterDir.exists("audio"))
    m_reciterDir.mkdir("audio");

  m_reciterDir.cd("audio");
  foreach (const Reciter& r, m_recitersList) {
    if (!m_reciterDir.exists(r.baseDirName)) {
      m_reciterDir.mkdir(r.baseDirName);
    }
  }

  // Connectors
  connect(this,
          &QMediaPlayer::mediaStatusChanged,
          this,
          &VersePlayer::verseStateChanged,
          Qt::UniqueConnection);

  connect(this,
          &VersePlayer::verseNoChanged,
          this,
          &VersePlayer::playCurrentVerse,
          Qt::UniqueConnection);

  m_reciterDir.cd(m_recitersList.at(m_reciter).baseDirName);
  setVerseFile(constructVerseFilename());
}

void
VersePlayer::fillRecitersList()
{
  m_reciterDir.cd("audio");

  Reciter husary{ "Al-Husary",
                  tr("Al-Husary"),
                  m_reciterDir.filePath("bismillah/husary.mp3"),
                  "https://everyayah.com/data/Husary_64kbps/" };

  Reciter husaryQasr{
    "Al-Husary_(Qasr)",
    tr("Al-Husary (Qasr)"),
    husary.basmallahPath,
    "https://github.com/0xzer0x/quran-companion/raw/audio-files/audio/"
    "husary_qasr_64kbps/"
  };

  Reciter husaryMujawwad{
    "Al-Husary_(Mujawwad)",
    tr("Al-Husary (Mujawwad)"),
    husary.basmallahPath,
    "https://everyayah.com/data/Husary_Mujawwad_64kbps/"
  };

  Reciter abdulbasit{
    "Abdul-Basit",
    tr("Abdul-Basit"),
    m_reciterDir.filePath("bismillah/abdul-basit.mp3"),
    "https://everyayah.com/data/Abdul_Basit_Murattal_64kbps/"
  };

  Reciter abdulbaitMujawwad{
    "Abdul-Basit_(Mujawwad)",
    tr("Abdul-Basit (Mujawwad)"),
    abdulbasit.basmallahPath,
    "https://everyayah.com/data/Abdul_Basit_Mujawwad_128kbps/"
  };

  Reciter menshawi{ "Menshawi",
                    tr("Menshawi"),
                    m_reciterDir.filePath("bismillah/menshawi.mp3"),
                    "https://everyayah.com/data/Minshawy_Murattal_128kbps/" };

  Reciter menshawiMujawwad{
    "Menshawi_(Mujawwad)",
    tr("Menshawi (Mujawwad)"),
    menshawi.basmallahPath,
    "https://everyayah.com/data/Minshawy_Mujawwad_64kbps/"
  };

  Reciter alafasy{ "Mishary_Alafasy",
                   tr("Mishary Alafasy"),
                   m_reciterDir.filePath("bismillah/alafasy.mp3"),
                   "https://everyayah.com/data/Alafasy_64kbps/" };

  Reciter tunaiji{ "Khalefa_Al-Tunaiji",
                   tr("Khalefa Al-Tunaiji"),
                   m_reciterDir.filePath("bismillah/tunaiji.mp3"),
                   "https://quran.ksu.edu.sa/ayat/mp3/tunaiji_64kbps/" };

  Reciter dussary{ "Yasser_Ad-Dussary",
                   tr("Yasser Ad-Dussary"),
                   m_reciterDir.filePath("bismillah/ad-dussary.mp3"),
                   "https://everyayah.com/data/Yasser_Ad-Dussary_128kbps/" };

  Reciter banna{ "Mahmoud_Al-Banna",
                 tr("Mahmoud Al-Banna"),
                 m_reciterDir.filePath("bismillah/al-banna.mp3"),
                 "https://everyayah.com/data/mahmoud_ali_al_banna_32kbps/" };

  Reciter basfar{ "Abdullah_Basfar",
                  tr("Abdullah Basfar"),
                  m_reciterDir.filePath("bismillah/basfar.mp3"),
                  "https://everyayah.com/data/Abdullah_Basfar_64kbps/" };

  Reciter shatree{ "Ash-Shaatree",
                   tr("Abu Bakr Ash-Shaatree"),
                   m_reciterDir.filePath("bismillah/shatree.mp3"),
                   "https://everyayah.com/data/Abu_Bakr_Ash-Shaatree_64kbps/" };

  Reciter ajamy{ "Al-Ajamy",
                 tr("Ahmed Al-Ajamy"),
                 m_reciterDir.filePath("bismillah/ajamy.mp3"),
                 "https://everyayah.com/data/ahmed_ibn_ali_al_ajamy_128kbps/" };

  Reciter aliJaber{ "Ali_Jaber",
                    tr("Ali Jaber"),
                    m_reciterDir.filePath("bismillah/ajaber.mp3"),
                    "https://everyayah.com/data/Ali_Jaber_64kbps/" };

  Reciter fAbbad{ "Fares_Abbad",
                  tr("Fares Abbad"),
                  m_reciterDir.filePath("bismillah/fabbad.mp3"),
                  "https://everyayah.com/data/Fares_Abbad_64kbps/" };

  Reciter ghamadi{ "Ghamadi",
                   tr("Saad Al-Ghamadi"),
                   m_reciterDir.filePath("bismillah/ghamadi.mp3"),
                   "https://everyayah.com/data/Ghamadi_40kbps/" };

  Reciter hRifai{ "Hani_Rifai",
                  tr("Hani Rifai"),
                  m_reciterDir.filePath("bismillah/rifai.mp3"),
                  "https://everyayah.com/data/Hani_Rifai_64kbps/" };

  Reciter hudhaify{ "Hudhaify",
                    tr("Hudhaify"),
                    m_reciterDir.filePath("bismillah/hudhaify.mp3"),
                    "https://everyayah.com/data/Hudhaify_64kbps/" };

  Reciter shuraym{ "Saood_Ash-Shuraym",
                   tr("Saood Ash-Shuraym"),
                   m_reciterDir.filePath("bismillah/shuraym.mp3"),
                   "https://everyayah.com/data/Saood_ash-Shuraym_64kbps/" };

  Reciter alqatami{ "Nasser_Alqatami",
                    tr("Nasser Alqatami"),
                    m_reciterDir.filePath("bismillah/qatami.mp3"),
                    "https://everyayah.com/data/Nasser_Alqatami_128kbps/" };

  Reciter muaiqly{ "Maher_AlMuaiqly",
                   tr("Maher Al-Muaiqly"),
                   m_reciterDir.filePath("bismillah/muaiqly.mp3"),
                   "https://everyayah.com/data/Maher_AlMuaiqly_64kbps/" };

  Reciter mIsmail{
    "Mostafa_Ismail",
    tr("Mostafa Ismail"),
    m_reciterDir.filePath("bismillah/mismail.mp3"),
    "https://quran.ksu.edu.sa/ayat/mp3/Mostafa_Ismail_128kbps/"
  };

  Reciter mJibreel{
    "Muhammad_Jibreel",
    tr("Muhammad Jibreel"),
    m_reciterDir.filePath("bismillah/mjibreel.mp3"),
    "https://quran.ksu.edu.sa/ayat/mp3/Muhammad_Jibreel_64kbps/"
  };

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
  m_recitersList.append(basfar);
  m_recitersList.append(shatree);
  m_recitersList.append(ajamy);
  m_recitersList.append(aliJaber);
  m_recitersList.append(fAbbad);
  m_recitersList.append(ghamadi);
  m_recitersList.append(hRifai);
  m_recitersList.append(hudhaify);
  m_recitersList.append(shuraym);
  m_recitersList.append(alqatami);
  m_recitersList.append(muaiqly);
  m_recitersList.append(mIsmail);
  m_recitersList.append(mJibreel);

  m_reciterDir.cdUp();
}

void
VersePlayer::setVerse(Verse& newVerse)
{
  m_activeVerse = newVerse;
}

/*!
 * \brief VersePlayer::nextVerse method to increment the verse & surah variables
 * appropriately according to the surah. emits a signal on verse change & on
 * changing from surah to another
 */
void
VersePlayer::nextVerse()
{
  // if last verse in surah, play basmalah before playing first verse
  if (m_activeVerse.number == m_surahCount) {
    // if last verse in surah an-nas (114), do nothing (i.e stop playback)
    if ((m_activeVerse.surah < 114)) {
      m_activeVerse.surah++;
      m_activeVerse.number = 1;
      updateSurahVerseCount(); // set new surah verse count
      emit surahChanged();     // signals surah change
    }

  } else {
    m_activeVerse.number++;
    emit verseNoChanged(); // signals active verse change
  }
}

/*!
 * \brief VersePlayer::verseStateChanged slot to call the nextVerse() method on
 * verse audio end \param status status of the media file, refer to QMediaPlayer
 * docs for enum.
 */
void
VersePlayer::verseStateChanged(QMediaPlayer::MediaStatus status)
{
  if (status == QMediaPlayer::EndOfMedia)
    nextVerse();
}

void
VersePlayer::changeUsedAudioDevice(QAudioDevice dev)
{
  m_output->setDevice(dev);
  setAudioOutput(m_output);
}

QString
VersePlayer::constructVerseFilename()
{
  // construct verse mp3 filename e.g. 002005.mp3
  QString filename;
  filename.append(QString::number(m_activeVerse.surah).rightJustified(3, '0'));
  filename.append(QString::number(m_activeVerse.number).rightJustified(3, '0'));

  filename.append(".mp3");
  return filename;
}

void
VersePlayer::playCurrentVerse()
{
  QString filename = constructVerseFilename();

  // set the attribute / source for QMediaPlayer
  if (setVerseFile(filename))
    // start playback of audio
    play();
}

void
VersePlayer::playBasmalah()
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

bool
VersePlayer::changeReciter(int reciterIdx)
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

bool
VersePlayer::setVerseFile(const QString& newVerseFilename)
{
  if (!m_reciterDir.exists(newVerseFilename)) {
    setSource(QUrl());
    qDebug() << "file " + newVerseFilename + " is missing.";
    emit missingVerseFile(m_reciter, m_activeVerse.surah);
    return false;
  }

  m_verseFile = newVerseFilename;
  setSource(QUrl::fromLocalFile(m_reciterDir.filePath(m_verseFile)));

  return true;
}

void
VersePlayer::updateSurahVerseCount()
{
  m_surahCount = m_dbMgr->getSurahVerseCount(m_activeVerse.surah);
}

/* -------------------- Getters ----------------------- */

QAudioOutput*
VersePlayer::getOutput() const
{
  return m_output;
}

QString
VersePlayer::verseFilename() const
{
  return m_verseFile;
}

int
VersePlayer::surahCount() const
{
  return m_surahCount;
}

Verse
VersePlayer::activeVerse() const
{
  return m_activeVerse;
}

QList<Reciter>
VersePlayer::recitersList() const
{
  return m_recitersList;
}
