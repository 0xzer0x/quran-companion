#include "configuration.h"
#include <QApplication>
#include <QFont>
#include <QKeySequence>
#include <QLocale>
#include <QStyleHints>
#include <QTranslator>
#include <utils/dirmanager.h>

Configuration&
Configuration::getInstance()
{
  static Configuration config;
  return config;
}

Configuration::Configuration()
  : m_settings(DirManager::getInstance().configDir().absoluteFilePath(
                 "qurancompanion.conf"),
               QSettings::IniFormat)
{
  checkGroups();
  setCustomPaths();
  m_themeId = m_settings.value("Theme").toInt();
  m_qcfVersion = m_settings.value("Reader/QCF").toInt();
  m_language = qvariant_cast<QLocale::Language>(m_settings.value("Language"));
  m_readerMode = qvariant_cast<ConfigurationSchema::ReaderMode>(
    m_settings.value("Reader/Mode"));
  m_darkMode = m_themeId == 2;
}

void
Configuration::setCustomPaths()
{
  QString path = m_settings.value("DownloadsDir").toString();
  if (!path.isEmpty()) {
    QDir dir = QDir(path);
    DirManager::getInstance().setDownloadsDir(dir);
  }
  DirManager::getInstance().createDirSkeleton();
}

void
Configuration::checkGroups()
{
  const ConfigurationSchema& schema = ConfigurationSchema::getInstance();
  QList<QString> allKeys = schema.getKeys().value();
  for (const QString& fullKey : allKeys) {
    QString settingsKey = fullKey;
    if (settingsKey.startsWith("General/")) {
      settingsKey.remove(0, 8);
    }
    m_settings.setValue(
      settingsKey,
      m_settings.value(settingsKey, schema.getDefault(fullKey).value()));
  }
}

void
Configuration::loadUiTranslation()
{
  if (m_language == QLocale::English)
    return;

  QString code = QLocale::languageToCode(m_language);
  QTranslator *translation = new QTranslator(qApp),
              *qtBase = new QTranslator(qApp);

  if (translation->load(":/i18n/qc_" + code + ".qm")) {
    qInfo() << translation->language() << "translation loaded";
    qInfo() << "base translation:" << qtBase->load(":/qtbase/" + code + ".qm");
    qApp->installTranslator(translation);
    qApp->installTranslator(qtBase);
  } else {
    qWarning() << code + " translation not loaded!";
    delete translation;
    delete qtBase;
  }

  if (m_language == QLocale::Arabic) {
    qApp->setLayoutDirection(Qt::RightToLeft);
  }
}

QSettings&
Configuration::settings()
{
  return m_settings;
}

const int
Configuration::themeId() const
{
  return m_themeId;
}

const bool
Configuration::darkMode() const
{
  return m_darkMode;
}

const int
Configuration::qcfVersion() const
{
  return m_qcfVersion;
}

const int
Configuration::qcfFontSize() const
{
  return m_settings
    .value("Reader/QCF" + QString::number(m_qcfVersion) + "Size", 22)
    .toInt();
}

const int
Configuration::verseFontSize() const
{
  return m_settings.value("Reader/VerseFontSize").toInt();
}

const QLocale::Language
Configuration::language() const
{
  return m_language;
}

const ConfigurationSchema::ReaderMode
Configuration::readerMode() const
{
  return m_readerMode;
}

const ConfigurationSchema::VerseType
Configuration::verseType() const
{
  return qvariant_cast<ConfigurationSchema::VerseType>(
    m_settings.value("Reader/VerseType"));
}

const Reciter
Configuration::reciter() const
{
  const QString reciterId = m_settings.value("Reciter").toString();
  // NOTE: Get reciter with the configured id and fallback if ID is not valid
  std::optional<Reciter> reciter = Reciter::findById(reciterId);
  return reciter.value_or(Reciter::defaultReciter());
}

const QByteArray
Configuration::windowState() const
{
  return m_settings.value("Window/State").toByteArray();
}

const bool
Configuration::menuBarVisible() const
{
  return m_settings.value("Window/VisibleMenubar").toBool();
}

const int
Configuration::khatmahId() const
{
  return m_settings.value("Reader/Khatmah").toInt();
}

const bool
Configuration::adaptiveReaderFont() const
{
  return m_settings.value("Reader/AdaptiveFont").toBool();
}

const QKeySequence
Configuration::shortcutKeySequence(const QString name) const
{
  QKeySequence seq =
    qvariant_cast<QKeySequence>(m_settings.value("Shortcuts/" + name));
  return seq;
}

const Tafsir
Configuration::tafsir() const
{
  const QString tafsirId = m_settings.value("Reader/Tafsir").toString();
  const std::optional<const Tafsir> tafsir = Tafsir::findById(tafsirId);
  return tafsir.value_or(Tafsir::defaultTafsir());
}

const Translation
Configuration::translation() const
{
  const QString translationId =
    m_settings.value("Reader/Translation").toString();
  const std::optional<const Translation> translation =
    Translation::findById(translationId);
  return translation.value_or(Translation::defaultTranslation());
}

const bool
Configuration::foregroundHighlighting() const
{
  return m_settings.value("Reader/FGHighlight").toBool();
}

const QFont
Configuration::sideContentFont() const
{
  return qvariant_cast<QFont>(m_settings.value("Reader/SideContentFont"));
}

const bool
Configuration::missingFileWarning() const
{
  return m_settings.value("MissingFileWarning").toBool();
}

const bool
Configuration::showVerseOfTheDay() const
{
  return m_settings.value("VOTD").toBool();
}

void
Configuration::setWindowState(const QByteArray windowState)
{
  m_settings.setValue("Window/State", windowState);
}

void
Configuration::setTafsir(const Tafsir tafsir)
{
  m_settings.setValue("Reader/Tafsir", tafsir.id());
}

void
Configuration::setTranslation(Translation translation)
{
  m_settings.setValue("Reader/Translation", translation.id());
}

void
Configuration::setAdaptiveReaderFont(const bool enabled)
{
  m_settings.setValue("Reader/AdaptiveFont", enabled);
}

void
Configuration::setKhatmahId(const int id)
{
  m_settings.setValue("Reader/Khatmah", id);
}

void
Configuration::setMenuBarVisible(const bool visible)
{
  m_settings.setValue("Window/VisibleMenubar", visible);
}

void
Configuration::setReciter(const Reciter reciter)
{
  m_settings.setValue("Reciter", reciter.id());
}

void
Configuration::setVerseType(ConfigurationSchema::VerseType newVerseType)
{
  m_settings.setValue("Reader/VerseType", newVerseType);
}

void
Configuration::setShortcutIfNotExists(const QString name, const QString value)
{
  QString settingsKey = "Shortcuts/" + name;
  if (!m_settings.contains(settingsKey)) {
    m_settings.setValue(settingsKey, value);
  }
}

void
Configuration::setQcfVersion(const int version)
{
  if (version == 1 || version == 2) {
    m_settings.setValue("Reader/QCF", version);
  }
}

void
Configuration::setQcfFontSize(const int size)
{
  m_settings.setValue("Reader/QCF" + QString::number(m_qcfVersion) + "Size",
                      size);
}

void
Configuration::setForegroundHighlighting(const bool enabled)
{
  m_settings.setValue("Reader/FGHighlight", enabled);
}

void
Configuration::setSideContentFont(const QFont font)
{
  m_settings.setValue("Reader/SideContentFont", font);
}

void
Configuration::setVerseFontSize(const int size)
{
  m_settings.setValue("Reader/VerseFontSize", size);
}

void
Configuration::setTheme(const int themeId)
{
  m_settings.setValue("Theme", themeId);
}

void
Configuration::setLanguage(const QLocale::Language lang)
{
  m_settings.setValue("Language", lang);
}

void
Configuration::setDownloadsDir(const QString path)
{
  m_settings.setValue("DownloadsDir", path);
}

void
Configuration::setShowVerseOfTheDay(const bool enabled)
{
  m_settings.setValue("VOTD", enabled);
}

void
Configuration::setMissingFileWarning(const bool enabled)
{
  m_settings.setValue("MissingFileWarning", enabled);
}

void
Configuration::setReaderMode(const ConfigurationSchema::ReaderMode mode)
{
  m_settings.setValue("Reader/Mode", mode);
}

void
Configuration::setShortcutKeySequence(const QString name,
                                      const QKeySequence sequence)
{
  QString settingsKey = "Shortcuts/" + name;
  m_settings.setValue(settingsKey, sequence.toString());
}

void
Configuration::sync()
{
  m_settings.sync();
}
