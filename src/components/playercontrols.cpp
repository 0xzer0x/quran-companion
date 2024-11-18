#include "playercontrols.h"
#include "ui_playercontrols.h"
#include <QtAwesome.h>
#include <algorithm>
#include <utils/shortcuthandler.h>
#include <utils/stylemanager.h>
using namespace fa;

PlayerControls::PlayerControls(QWidget* parent,
                               QPointer<PlaybackController> playbackController,
                               QPointer<QuranReader> reader,
                               QPointer<RepeaterPopup> repeater)
  : QWidget(parent)
  , ui(new Ui::PlayerControls)
  , m_reader(reader)
  , m_repeater(repeater)
  , m_playbackController(playbackController)
  , m_currVerse(Verse::getCurrent())
  , m_config(Configuration::getInstance())
  , m_reciters(Reciter::reciters)
{
  ui->setupUi(this);
  loadIcons();
  setupConnections();

  foreach (const Reciter& r, m_reciters)
    ui->cmbReciter->addItem(r.displayName());

  ui->cmbReciter->setCurrentIndex(
    m_config.settings().value("Reciter", 0).toInt());
}

void
PlayerControls::loadIcons()
{
  fa::QtAwesome& awesome = StyleManager::getInstance().awesome();
  ui->btnPlay->setIcon(awesome.icon(fa_solid, fa_play));
  ui->btnPause->setIcon(awesome.icon(fa_solid, fa_pause));
  ui->btnStop->setIcon(awesome.icon(fa_solid, fa_stop));
  ui->btnRepeat->setIcon(awesome.icon(fa_solid, fa_rotate_right));

  ui->lbSpeaker->setText(QString(fa_volume_high));
  ui->lbSpeaker->setFont(awesome.font(fa_solid, 16));
}

void
PlayerControls::setupConnections()
{
  const ShortcutHandler& handler = ShortcutHandler::getInstance();
  connect(&handler,
          &ShortcutHandler::togglePlayback,
          this,
          &PlayerControls::togglePlayback);
  connect(&handler,
          &ShortcutHandler::incrementVolume,
          this,
          &PlayerControls::incrementVolume);
  connect(&handler,
          &ShortcutHandler::decrementVolume,
          this,
          &PlayerControls::decrementVolume);

  connect(m_playbackController->player(),
          &QMediaPlayer::positionChanged,
          this,
          &PlayerControls::mediaPosChanged);
  connect(m_playbackController->player(),
          &QMediaPlayer::playbackStateChanged,
          this,
          &PlayerControls::mediaStateChanged);

  connect(ui->sldrAudioPlayer,
          &QSlider::sliderMoved,
          m_playbackController->player(),
          &QMediaPlayer::setPosition);
  connect(ui->sldrVolume,
          &QSlider::valueChanged,
          this,
          &PlayerControls::volumeSliderValueChanged);

  connect(
    ui->btnPlay, &QPushButton::clicked, this, &PlayerControls::btnPlayClicked);
  connect(ui->btnPause,
          &QPushButton::clicked,
          this,
          &PlayerControls::btnPauseClicked);
  connect(
    ui->btnStop, &QPushButton::clicked, this, &PlayerControls::btnStopClicked);

  connect(ui->cmbReciter,
          &QComboBox::currentIndexChanged,
          this,
          &PlayerControls::cmbReciterChanged);

  connect(ui->btnRepeat,
          &QPushButton::toggled,
          this,
          &PlayerControls::btnRepeatClicked);
}

void
PlayerControls::togglePlayback()
{
  if (m_playbackController->player()->isPlaying()) {
    btnPauseClicked();
  } else {
    btnPlayClicked();
  }
}

void
PlayerControls::mediaPosChanged(qint64 position)
{
  if (ui->sldrAudioPlayer->maximum() !=
      m_playbackController->player()->duration())
    ui->sldrAudioPlayer->setMaximum(m_playbackController->player()->duration());

  if (!ui->sldrAudioPlayer->isSliderDown())
    ui->sldrAudioPlayer->setValue(position);
}

void
PlayerControls::btnPlayClicked()
{
  m_reader->highlightCurrentVerse();
  m_playbackController->player()->play();
}

void
PlayerControls::btnPauseClicked()
{
  m_playbackController->player()->pause();
}

void
PlayerControls::btnStopClicked()
{
  m_playbackController->stop();
}

void
PlayerControls::btnRepeatClicked(bool on)
{
  if (on) {
    m_repeater->adjustPosition();
    m_repeater->show();
  } else {
    m_repeater->hide();
  }
}

void
PlayerControls::cmbReciterChanged(int newIndex)
{
  m_playbackController->player()->changeReciter(newIndex);
  m_repeater->playbackFinished();
}

void
PlayerControls::mediaStateChanged(QMediaPlayer::PlaybackState state)
{
  if (state == QMediaPlayer::PlayingState) {
    ui->btnPlay->setEnabled(false);
    ui->btnPause->setEnabled(true);
    ui->btnStop->setEnabled(true);
  } else if (state == QMediaPlayer::PausedState) {
    ui->btnPlay->setEnabled(true);
    ui->btnPause->setEnabled(false);
    ui->btnStop->setEnabled(true);
  } else if (state == QMediaPlayer::StoppedState) {
    ui->btnPlay->setEnabled(true);
    ui->btnPause->setEnabled(false);
    ui->btnStop->setEnabled(false);
  }
}

void
PlayerControls::volumeSliderValueChanged(int position)
{
  qreal linearVolume =
    QAudio::convertVolume(ui->sldrVolume->value() / qreal(100.0),
                          QAudio::LogarithmicVolumeScale,
                          QAudio::LinearVolumeScale);
  if (linearVolume != m_volume) {
    m_volume = linearVolume;
    m_playbackController->player()->setPlayerVolume(m_volume);
  }
}

void
PlayerControls::incrementVolume()
{
  int val = ui->sldrVolume->value() + 5;
  ui->sldrVolume->setValue(val > 100 ? 100 : val);
}

void
PlayerControls::decrementVolume()
{
  int val = ui->sldrVolume->value() - 5;
  ui->sldrVolume->setValue(val < 0 ? 0 : val);
}

int
PlayerControls::currentReciter() const
{
  return ui->cmbReciter->currentIndex();
}

void
PlayerControls::adjustWidth()
{
  QFrame* readerFrame = m_reader->findChild<QFrame*>("frmReader");
  this->setMaximumWidth(std::min(readerFrame->width(), 950));
}

void
PlayerControls::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);
  ui->btnRepeat->setChecked(false);
  m_repeater->hide();
}

PlayerControls::~PlayerControls()
{
  delete ui;
}
