#include "content.h"

Content::Content(QString id, QString display, QString filename, bool isExtra)
  : m_id(id)
  , m_displayName(display)
  , m_filename(filename)
  , m_isExtra(isExtra)
{
}

const QString&
Content::id() const
{
  return m_id;
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
