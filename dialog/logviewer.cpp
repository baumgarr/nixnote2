#include "logviewer.h"
#include "global.h"

#include <QTextEdit>
#include <QFileDialog>
#include <QTextStream>

extern Global global;

LogViewer::LogViewer(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    this->setLayout(mainLayout);

    viewer = new QTextEdit();
    viewer->setReadOnly(true);
    viewer->setLineWrapMode(QTextEdit::NoWrap);
    mainLayout->addWidget(viewer);

    okButton = new QPushButton();
    okButton->setText(tr("Close"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
    refreshButton = new QPushButton(tr("Refresh"));
    connect(refreshButton, SIGNAL(clicked()), this, SLOT(loadData()));
    saveButton = new QPushButton(tr("Save"));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveData()));

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(okButton);
    mainLayout->addLayout(buttonLayout);
    loadData();
    this->setWindowTitle(tr("Save Logs"));
    this->resize(600,400);
    this->setFont(global.getGuiFont(font()));
}


void LogViewer::loadData() {
    viewer->clear();
    QString fileName = global.fileManager.getLogsDirPath("") + "messages.log";
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line.replace("\n", "");
            if (line.startsWith("DEBUG"))
                line = "<span style=\"color:gray;\">" +line +"</span>";
            if (line.startsWith("TRACE"))
                line = "<span style=\"color:blue;\">" +line +"</span>";
            if (line.startsWith("ERROR"))
                line = "<span style=\"color:red;\">" +line +"</span>";
            viewer->append(line);
        }
    }
    file.close();
}


void LogViewer::saveData() {
    QFileDialog fd;
    fd.setFileMode(QFileDialog::AnyFile);
    fd.setConfirmOverwrite(true);
    fd.setWindowTitle(tr("Save Log"));
    fd.setFilter(tr("NixNote Log (*.log);;All Files (*.*)"));
    fd.setAcceptMode(QFileDialog::AcceptSave);
        fd.setDirectory(QDir::homePath());
    if (fd.exec() == 0 || fd.selectedFiles().size() == 0) {
        return;
    }

    QString infileName = global.fileManager.getLogsDirPath("") + "messages.log";
    QStringList fileNames;
    fileNames = fd.selectedFiles();
    QFile infile(infileName);
    QFile outfile(fileNames[0]);
    if (infile.open(QIODevice::ReadOnly) && outfile.open(QIODevice::WriteOnly)) {
        QTextStream out(&outfile);
        while (!infile.atEnd()) {
            QString line = infile.readLine();
            out << line;
        }
    }
    infile.close();


}
