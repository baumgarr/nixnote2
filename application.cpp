#include "application.h"
#include <QDebug>

Application::Application(int &argc, char** argv) :
    QApplication(argc, argv)
{
}


bool Application::notify(QObject *receiver_, QEvent *event_)
{
  try
  {
    return QApplication::notify(receiver_, event_);
  }
  catch (std::exception &ex)
  {
        qDebug() << "std::exception was caught: " << ex.what();
  }

  return false;
}
