#ifndef NUMBERTOSTRINGCONVERTER_H
#define NUMBERTOSTRINGCONVERTER_H

#include <QHash>
#include <QString>

class NumberToStringConverter
{
public:
  NumberToStringConverter();

  QString arabicNumber(int number);

private:
  /**
   * @brief Hash Table used for converting page number to arabic numbers
   */
  const QHash<int, QString> m_easternNumsMap;
};

#endif // NUMBERTOSTRINGCONVERTER_H
