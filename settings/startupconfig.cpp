#include "startupconfig.h"
#include <QDir>


StartupConfig::StartupConfig()
{
    homeDirPath = QDir().homePath() + QString("/.nixnote/");
}
