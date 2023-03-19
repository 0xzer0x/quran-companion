#ifndef VERSEPLAYER_H
#define VERSEPLAYER_H

#include <QApplication>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QDir>
#include <QMediaPlayer>
#include <QObject>
#include "dbmanager.h"

/*!
 * \class VersePlayer
 * 
 * \brief The VersePlayer class is responsible for the playback of verses audio files
 */
class VersePlayer : public QMediaPlayer
{
    Q_OBJECT

private:
    int m_surahIdx;
    int m_verseNum;
    int m_surahCount = 0;
    int m_reciter;
    QString m_verseFile;
    QDir m_reciterDir;
    QAudioOutput *m_output;
    DBManager *m_dbPtr;
    QList<QString> m_reciterDirNames;
    QList<QString> m_bsmlPaths;

public:
    explicit VersePlayer(QObject *parent = nullptr,
                         DBManager *dbPtr = nullptr,
                         int initSurah = 1,
                         int initVerse = 1);
    void setSurahIdx(int newSurahIdx);
    void setVerseNum(int newVerseNum);
    QString verseFilename() const;
    bool setVerseFile(const QString &newVerseFilename);
    QString constructVerseFilename();
    void updateSurahVerseCount();
    int surahIdx() const;
    int surahCount() const;
    int verseNum() const;
    QAudioOutput *getOutput() const;

    QList<QString> reciterDirNames() const;

public slots:
    void playBasmalah();
    void nextVerse();
    void playCurrentVerse();
    bool changeReciter(int reciterIdx);
    void verseStateChanged(QMediaPlayer::MediaStatus status);
    void changeUsedAudioDevice(QAudioDevice dev);

signals:
    void newSurah();
    void newVerse();
    void missingVerseFile();
};

#endif // VERSEPLAYER_H
