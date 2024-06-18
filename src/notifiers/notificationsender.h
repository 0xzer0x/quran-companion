#ifndef NOTIFICATIONSENDER_H
#define NOTIFICATIONSENDER_H

#include <QObject>

class NotificationSender : public QObject
{
  Q_OBJECT
public:
  /**
   * @brief The Type enum represents all possible action to notify the user of
   */
  enum Type
  {
    info,           ///< general information
    success,        ///< successful operation
    fail,           ///< failed operation
    bookmarkAdd,    ///< bookmark addition
    bookmarkRemove, ///< bookmark removal
    copiedText,     ///< verse text copied
    updateInfo      ///< version information
  };
signals:
  void notify(Type type, QString msg);
};

#endif // NOTIFICATIONSENDER_H
