#ifndef CONFIGURATIONSCHEMA_H
#define CONFIGURATIONSCHEMA_H

#include <QHash>
#include <QString>
#include <optional>
using std::nullopt;
using std::optional;

class ConfigurationSchema
{
public:
  enum VerseType
  {
    Qcf,
    Hafs,
    HafsAnnotated,
    Warsh
  };
  /**
   * @brief ReaderMode enum represents the available modes for the Quran reader
   * in MainWindow
   */
  enum ReaderMode
  {
    SinglePage, ///< Single Quran page, side panel is used for displaying verses
                ///< with translation
    DoublePage  ///< Two Quran pages, both panels are used to display Quran
                ///< pages, no translation
  };

  static ConfigurationSchema& getInstance();
  QList<QString> getGroups() const;
  optional<QList<QString>> getKeys(optional<QString> group = nullopt) const;
  optional<QVariant> getDefault(QString key) const;

private:
  ConfigurationSchema();
  QHash<QString, QHash<QString, QVariant>> m_schema;
};

#endif // CONFIGURATIONSCHEMA_H
