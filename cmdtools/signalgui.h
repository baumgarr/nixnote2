#ifndef SIGNALGUI_H
#define SIGNALGUI_H

#include <QObject>

class SignalGui : public QObject
{
    Q_OBJECT
public:
    SignalGui();
    bool takeScreenshot;
    bool openNote;
    bool openExternalNote;
    bool openNoteNewTab;
    bool newExternalNote;
    bool newNote;
    bool shutdown;
    bool synchronize;
    bool show;
    qint32 lid;
};

#endif // SIGNALGUI_H
