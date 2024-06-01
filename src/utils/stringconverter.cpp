#include "stringconverter.h"

StringConverter::StringConverter()
  : m_easternNumsMap({ { 0, "٠" },
                       { 1, "١" },
                       { 2, "٢" },
                       { 3, "٣" },
                       { 4, "٤" },
                       { 5, "٥" },
                       { 6, "٦" },
                       { 7, "٧" },
                       { 8, "٨" },
                       { 9, "٩" } })
{
}

QString
StringConverter::arabicNumber(int number)
{
  if (number < 10)
    return m_easternNumsMap.value(number);

  QString easternNum;
  while (number > 0) {
    easternNum.insert(0, m_easternNumsMap.value(number % 10));
    number /= 10;
  }
  return easternNum;
}
