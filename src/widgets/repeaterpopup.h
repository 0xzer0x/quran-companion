#ifndef REPEATERPOPUP_H
#define REPEATERPOPUP_H

#include "navigation/navigator.h"
#include "navigation/verseobserver.h"
#include "player/impl/setplaybackstrategy.h"
#include <QIntValidator>
#include <QWidget>
#include <QtAwesome.h>
#include <player/playbackcontroller.h>
#include <player/playbackstrategy.h>
#include <qvalidator.h>
#include <service/quranservice.h>
#include <types/verse.h>
using namespace std;

namespace Ui {
class RepeaterPopup;
}

class RepeaterPopup
  : public QWidget
  , public VerseObserver
{
  Q_OBJECT

public:
  explicit RepeaterPopup(QWidget* parent,
                         QPointer<PlaybackController> playbackcontroller);
  ~RepeaterPopup();

  void activeVerseChanged() override;

  void show();

public slots:
  void playbackFinished();

private slots:
  void cmbSurahChanged(int newIndex);
  void btnPlaybackClicked(bool checked);

private:
  Ui::RepeaterPopup* ui;
  Verse& m_currVerse;
  Navigator& m_navigator;
  QuranService* m_quranService;
  fa::QtAwesome& m_awesome;
  void setupUI();
  void setToggleState(bool checked);
  void setValidatorsRange(int bottom, int top);
  void populateVerseComboboxes();
  void showErrorMessagebox(QString msg);
  optional<shared_ptr<SetPlaybackStrategy>> generatePlaybackStrategy();
  optional<shared_ptr<SetPlaybackStrategy>> m_playbackStrategy;
  QPointer<PlaybackController> m_playbackController;
  QIntValidator m_fromVerseValidator;
  QIntValidator m_toVerseValidator;
};

#endif // REPEATERPOPUP_H
