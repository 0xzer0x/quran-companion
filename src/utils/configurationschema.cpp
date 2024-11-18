#include <QFont>
#include <QGuiApplication>
#include <QLocale>
#include <QStyleHints>
#include <qforeach.h>
#include <utility>
#include <utils/configurationschema.h>
using std::make_pair;

ConfigurationSchema&
ConfigurationSchema::getInstance()
{
  static ConfigurationSchema configurationSchema;
  return configurationSchema;
}

ConfigurationSchema::ConfigurationSchema()
{
  QHash<QString, QVariant> general = {
    make_pair("Language", (int)QLocale::English),
    make_pair("Theme",
              QGuiApplication::styleHints()->colorScheme() ==
                  Qt::ColorScheme::Dark
                ? 2
                : 0),
    make_pair("VOTD", true),
    make_pair("MissingFileWarning", true),
    make_pair("DownloadsDir", QVariant()),
  };
  QHash<QString, QVariant> window = {
    make_pair("State", QVariant()),
    make_pair("VisibleMenubar", true),
  };
  QHash<QString, QVariant> reader = {
    make_pair("Mode", 0),
    make_pair("QCF", 1),
    make_pair("VerseType", 0),
    make_pair("VerseFontSize", 20),
    make_pair("FGHighlight", 1),
    make_pair("AdaptiveFont", true),
    make_pair("QCF1Size", 22),
    make_pair("QCF2Size", 20),
    make_pair("Khatmah", 0),
    make_pair("SideContentFont", QFont("Expo Arabic", 14)),
    make_pair("Tafsir", "sa3dy"),
    make_pair("Translation", "en_khattab"),
  };

  m_schema = {
    make_pair("General", general),
    make_pair("Window", window),
    make_pair("Reader", reader),
  };
}

optional<QVariant>
ConfigurationSchema::getDefault(QString key) const
{
  QString group = "General";
  if (key.contains("/")) {
    QStringList parts = key.split("/");
    group = parts.at(0);
    key = parts.at(1);
  }

  if (!m_schema.contains(group) || !m_schema.value(group).contains(key)) {
    return nullopt;
  }

  return m_schema.value(group).value(key);
}

optional<QList<QString>>
ConfigurationSchema::getKeys(optional<QString> group) const
{
  if (!group.has_value()) {
    QList<QString> allKeys;
    for (const auto& group : m_schema.keys()) {
      for (const auto& key : m_schema.value(group).keys()) {
        allKeys.append(QString(group + "/" + key));
      }
    }
    return allKeys;
  }

  if (!m_schema.contains(group.value())) {
    return nullopt;
  }

  return m_schema.value(group.value()).keys();
}

QList<QString>
ConfigurationSchema::getGroups() const
{
  return m_schema.keys();
}
