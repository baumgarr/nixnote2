#ifndef STARTUPCONFIG_H
#define STARTUPCONFIG_H

#include <QString>

class StartupConfig
{
public:
    StartupConfig();
    QString name;
    QString homeDirPath;
    QString programDirPath;
    int accountId;
};

#endif // STARTUPCONFIG_H
