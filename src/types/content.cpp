#include "content.h"

Content::Content(QString display, QString filename, bool isExtra)
  : m_displayName(display)
  , m_filename(filename)
  , m_isExtra(isExtra)
{
}

const QString&
Content::displayName() const
{
  return m_displayName;
}

const QString&
Content::filename() const
{
  return m_filename;
}

const bool&
Content::isExtra() const
{
  return m_isExtra;
}
