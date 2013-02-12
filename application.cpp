#include "application.h"
#include "global.h"
#include <QDebug>

extern Global global;

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
        QLOG_ERROR() << "std::exception was caught: " << ex.what();
  }

  return false;
}
