/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2015 Randy Baumgarte

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
***********************************************************************************/

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
//    QFileDialog fd;
    QFileDialog fd(0, tr("Save Log"), QDir::homePath(), tr("NixNote Log (*.log);;All Files (*.*)"));
    fd.setFileMode(QFileDialog::AnyFile);
    fd.setConfirmOverwrite(true);
//    fd.setWindowTitle(tr("Save Log"));
//    fd.setFilter(tr("NixNote Log (*.log);;All Files (*.*)"));
    fd.setAcceptMode(QFileDialog::AcceptSave);
//       fd.setDirectory(QDir::homePath());
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
