#ifndef STRINGCONVERTER_H
#define STRINGCONVERTER_H

#include <QHash>
#include <QString>

class StringConverter
{
public:
  StringConverter();

  QString arabicNumber(int number);

private:
  /**
   * @brief Hash Table used for converting page number to arabic numbers
   */
  const QHash<int, QString> m_easternNumsMap;
};

#endif // STRINGCONVERTER_H
