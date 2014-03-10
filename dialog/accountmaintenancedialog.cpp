#include "accountmaintenancedialog.h"
#include <QSpacerItem>
#include <QMessageBox>
#include <QList>
#include "global.h"
#include "dialog/adduseraccountdialog.h"

extern Global global;

AccountMaintenanceDialog::AccountMaintenanceDialog(NMainMenuBar *menubar, QWidget *parent) :
    QDialog(parent)
{
    this->parent = parent;
    this->menuBar = menubar;
    okButton = new QPushButton(tr("Ok"), this);
    closeButton = new QPushButton(tr("Close"),this);
    addButton = new QPushButton(tr("Add"),this);
    renameButton = new QPushButton(tr("Rename"));
    deleteButton = new QPushButton(tr("Delete"));
    nameList = new QListWidget(this);
    buttonLayout1 = new QVBoxLayout();
    buttonLayout2 = new QHBoxLayout();
    displayLayout = new QHBoxLayout();
    mainLayout = new QVBoxLayout();

    displayLayout->addWidget(nameList);
    displayLayout->addLayout(buttonLayout1);
    buttonLayout1->addWidget(addButton);
    buttonLayout1->addWidget(renameButton);
    buttonLayout1->addWidget(deleteButton);
    buttonLayout1->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Maximum));
    buttonLayout1->setStretch(4,100);
    mainLayout->addLayout(displayLayout);
    mainLayout->addLayout(buttonLayout2);

    buttonLayout2->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Maximum, QSizePolicy::Minimum));
    buttonLayout2->addWidget(okButton);
    buttonLayout2->addWidget(closeButton);
    buttonLayout2->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Maximum, QSizePolicy::Minimum));
    buttonLayout2->setStretch(0,100);
    buttonLayout2->setStretch(3,100);
    this->setLayout(mainLayout);
    this->setWindowTitle(tr("User Account Maintenance"));

    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(addAccount()));
    connect(deleteButton,SIGNAL(clicked()), this, SLOT(deleteAccount()));
    connect(renameButton, SIGNAL(clicked()), this, SLOT(renameAccount()));
    this->loadData();
}



void AccountMaintenanceDialog::loadData() {
    names.clear();
    ids.clear();
    nameList->clear();
    deleteButton->setEnabled(false);

    names = global.accountsManager->nameList();
    ids = global.accountsManager->idList();
    for (int i=0; i<names.size(); i++)
        nameList->addItem(names[i]);

    if (ids.size() >= 2) {
        deleteButton->setEnabled(true);
    }
    nameList->sortItems();

}


void AccountMaintenanceDialog::deleteAccount() {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(tr("Are you sure you want to delete this account?"));
    msgBox.setWindowTitle(tr("Verify Delete"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    if (ret == QMessageBox::No)
        return;
    int id = -1;
    QString name = nameList->selectedItems()[0]->text();
    for (int i=0; i<names.size(); i++) {
        if (names[i] == name) {
            if (ids[i] == global.accountsManager->currentId) {
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setText(tr("You cannot delete the active account."));
                msgBox.setWindowTitle("Error deleting account");
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.exec();
                return;
            }
            global.accountsManager->removeId(ids[i]);
            id = ids[i];
            i=names.size();
        }
    }
    loadData();
    if (id>-1) {
        for (int i=0; i<menuBar->userAccountActions.size(); i++) {
            QAction *action = menuBar->userAccountActions[i];
            if (action->data().toInt() == id) {
                action = menuBar->userAccountActions.takeAt(i);
                action->disconnect();
                menuBar->removeAction(action);
                delete action;
            }
        }
    }
    QFile configFile(global.fileManager.getHomeDirPath("")+"nixnote-"+QString::number(id) +".conf");
    configFile.remove();
    removeDir(global.fileManager.getHomeDirPath("")+QString("db-")+QString::number(id));
}

void AccountMaintenanceDialog::renameAccount() {
    int id = -1;
    bool activeId = false;
    QString name = nameList->selectedItems()[0]->text();
    QString newName;
    for (int i=0; i<names.size(); i++) {
        if (names[i] == name) {
            if (ids[i] == global.accountsManager->currentId)
                activeId = true;
            else
                activeId = false;
            AddUserAccountDialog userDialog;
            userDialog.newAccountName->setText(name);
            userDialog.exec();
            if (!userDialog.okPushed)
                return;
            newName = userDialog.newAccountName->text();
            global.accountsManager->setName(newName, ids[i]);
            id = ids[i];
            i=names.size();
        }
    }
    if (!activeId)
        newName = tr("Switch to ") + newName;
    if (id>-1) {
        for (int i=0; i<menuBar->userAccountActions.size(); i++) {
            QAction *action = menuBar->userAccountActions[i];
            if (action->data().toInt() == id) {
                action = menuBar->userAccountActions.at(i);
                action->setText(newName);
            }
        }
    }
    loadData();
}


void AccountMaintenanceDialog::addAccount() {
    AddUserAccountDialog dialog;
    dialog.exec();
    if (!dialog.okPushed)
        return;
    QString name = dialog.newAccountName->text().trimmed();
    int newid = global.accountsManager->addId(-1, name);
    QAction *newAction = new QAction(menuBar);
    newAction->setText(tr("Switch to ") +name);
    newAction->setCheckable(true);
    newAction->setData(newid);
    menuBar->addUserAccount(newAction);
    connect(newAction, SIGNAL(triggered()), (NixNote*)parent, SLOT(switchUser()));
    loadData();
}


bool AccountMaintenanceDialog:: removeDir(const QString & dirName)  {
    bool result = false;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}
