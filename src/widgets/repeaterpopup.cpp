#include "repeaterpopup.h"
#include "ui_repeaterpopup.h"
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QPointer>
#include <QtAwesome.h>
#include <player/impl/setplaybackstrategy.h>
#include <player/playbackstrategy.h>
#include <service/servicefactory.h>
#include <utils/stylemanager.h>

RepeaterPopup::RepeaterPopup(QWidget* parent,
                             QPointer<PlaybackController> playbackController)
  : QWidget(parent)
  , ui(new Ui::RepeaterPopup)
  , m_quranService(ServiceFactory::quranService())
  , m_currVerse(Verse::getCurrent())
  , m_navigator(Navigator::getInstance())
  , m_awesome(StyleManager::getInstance().awesome())
  , m_playbackController(playbackController)
  , m_playbackStrategy(nullopt)
{
  setupUI();
  m_navigator.addObserver(this);
}

void
RepeaterPopup::setupUI()
{
  this->hide();
  ui->setupUi(this);
  QPointer<QGraphicsDropShadowEffect> shadow =
    new QGraphicsDropShadowEffect(this);
  shadow->setOffset(0, 4);
  shadow->setBlurRadius(15);
  shadow->setColor(QColor(0, 0, 0, 60));
  setGraphicsEffect(shadow);
  setToggleState(false);

  QStringList surahNames = m_quranService->surahNames();
  ui->cmbFromSurah->addItems(surahNames);
  ui->cmbToSurah->addItems(surahNames);
  for (auto& cmb : { ui->cmbFromSurah, ui->cmbToSurah }) {
    connect(cmb, &QComboBox::activated, this, &RepeaterPopup::cmbSurahChanged);
  }

  connect(ui->btnTogglePlayback,
          &QPushButton::clicked,
          this,
          &RepeaterPopup::btnPlaybackClicked);
  connect(m_playbackController,
          &PlaybackController::playbackFinished,
          this,
          &RepeaterPopup::playbackFinished);
}

void
RepeaterPopup::showErrorMessagebox(QString msg)
{
  QMessageBox::warning(parentWidget(), "Invalid Value", msg);
}

void
RepeaterPopup::cmbSurahChanged(int newIndex)
{
  QIntValidator* validator = nullptr;
  QComboBox* cmb = nullptr;
  if (sender() == ui->cmbFromSurah) {
    cmb = ui->cmbFromVerse;
    validator = &m_fromVerseValidator;
  } else {
    cmb = ui->cmbToVerse;
    validator = &m_toVerseValidator;
  }

  cmb->clear();
  for (int i = 1; i <= Verse::surahVerseCount(newIndex + 1); i++)
    cmb->addItem(QString::number(i));
  validator->setTop(Verse::surahVerseCount(newIndex + 1));
  cmb->setCurrentIndex(0);
}

void
RepeaterPopup::btnPlaybackClicked(bool checked)
{
  if (checked) {
    m_playbackStrategy = generatePlaybackStrategy();
    if (m_playbackStrategy.has_value()) {
      m_playbackController->setStrategy(m_playbackStrategy.value());
      m_playbackController->start();
      ui->btnTogglePlayback->setIcon(m_awesome.icon(fa::fa_solid, fa::fa_stop));
    } else {
      ui->btnTogglePlayback->setChecked(false);
    }
  } else {
    m_playbackStrategy.reset();
    m_playbackController->stop();
    m_playbackController->resetStrategy();
    ui->btnTogglePlayback->setIcon(m_awesome.icon(fa::fa_solid, fa::fa_play));
  }
}

void
RepeaterPopup::playbackFinished()
{
  setToggleState(false);
  m_playbackStrategy.reset();
  m_playbackController->resetStrategy();
}

optional<shared_ptr<SetPlaybackStrategy>>
RepeaterPopup::generatePlaybackStrategy()
{
  int fromSurah = ui->cmbFromSurah->currentIndex() + 1;
  int fromVerse = ui->cmbFromVerse->currentIndex() + 1;
  Verse start = Verse(
    m_quranService->versePage(fromSurah, fromVerse), fromSurah, fromVerse);

  int toSurah = ui->cmbToSurah->currentIndex() + 1;
  int toVerse = ui->cmbToVerse->currentIndex() + 1;
  Verse end =
    Verse(m_quranService->versePage(toSurah, toVerse), toSurah, toVerse);

  int frequency = ui->spnVerseFrequency->value();
  int frequencyDelay = ui->spnVerseFrequencyDelay->value() * 1000;
  int repetitions = ui->spnRepetitions->value();

  if (start > end) {
    showErrorMessagebox(tr("End verse cannot be before start verse"));
    return nullopt;
  }
  if (frequency < 1 || repetitions < 1) {
    showErrorMessagebox("Verse frequency and repetitions must be more than 0");
    return nullopt;
  }

  return shared_ptr<SetPlaybackStrategy>(
    new SetPlaybackStrategy(start, end, repetitions, frequency, frequencyDelay));
}

void
RepeaterPopup::populateVerseComboboxes()
{
  ui->cmbFromVerse->clear();
  ui->cmbToVerse->clear();

  setValidatorsRange(1, m_currVerse.surahCount());
  for (int i = 1; i <= m_currVerse.surahCount(); i++) {
    ui->cmbFromVerse->addItem(QString::number(i));
    ui->cmbToVerse->addItem(QString::number(i));
  }

  for (QComboBox* const& cmb : { ui->cmbFromVerse, ui->cmbToVerse })
    cmb->setCurrentIndex(m_currVerse.number() - 1);
}

void
RepeaterPopup::setValidatorsRange(int bottom, int top)
{
  m_fromVerseValidator.setRange(bottom, top);
  m_toVerseValidator.setRange(bottom, top);
  ui->cmbFromVerse->setValidator(&m_fromVerseValidator);
  ui->cmbToVerse->setValidator(&m_toVerseValidator);
}

void
RepeaterPopup::activeVerseChanged()
{
  if (m_playbackStrategy.has_value() &&
      !m_playbackStrategy->get()->verseInRange(m_currVerse))
    playbackFinished();
}

void
RepeaterPopup::setToggleState(bool checked)
{
  ui->btnTogglePlayback->setChecked(checked);
  ui->btnTogglePlayback->setIcon(checked
                                   ? m_awesome.icon(fa::fa_solid, fa::fa_stop)
                                   : m_awesome.icon(fa::fa_solid, fa::fa_play));
}

void
RepeaterPopup::show()
{
  if (!m_playbackStrategy.has_value()) {
    ui->cmbFromSurah->setCurrentIndex(m_currVerse.surah() - 1);
    ui->cmbToSurah->setCurrentIndex(m_currVerse.surah() - 1);
    populateVerseComboboxes();
  }
  QWidget::show();
}

void
RepeaterPopup::adjustPosition()
{
  QPushButton* btnRepeat = parentWidget()->findChild<QPushButton*>("btnRepeat");
  if (!btnRepeat) {
    qCritical() << "Repeat button not found";
    return;
  }
  QPoint topLeft;
  if (layoutDirection() == Qt::LeftToRight) {
    topLeft = btnRepeat->mapTo(parentWidget(),
                               QPoint(btnRepeat->width(), btnRepeat->height()));
    topLeft.setX(topLeft.x() - width() + 10);
  } else {
    topLeft = btnRepeat->mapTo(parentWidget(), QPoint(0, btnRepeat->height()));
    topLeft.setX(topLeft.x() - 10);
  }
  topLeft.setY(topLeft.y() + 2);
  move(topLeft);
}

void
RepeaterPopup::moveEvent(QMoveEvent* event)
{
  QWidget::moveEvent(event);
  if (isVisible())
    adjustPosition();
}

RepeaterPopup::~RepeaterPopup()
{
  delete ui;
}
