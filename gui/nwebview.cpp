#include "nwebview.h"
#include "nwebpage.h"
#include "global.h"
#include <QWebFrame>
#include <QMessageBox>
#include "./libencrypt/encrypt_global.h"
#include "./libencrypt/encrypt.h"
#include "../dialog/endecryptdialog.h"

extern Global global;


//****************************************************
//* This inherits everything from QWebView.  It is
//* useful to allow us to setup things that are
//* needed to view & edit notes.
//****************************************************

// Override the constructor so we always use a NWebPage
// rather than a QWebPage
NWebView::NWebView(QWidget *parent) :
    QWebView(parent)
{
    editorPage = new NWebPage();
    setPage(editorPage);
    isDirty = false;
    connect(editorPage, SIGNAL(contentsChanged()), this, SLOT(editAlert()));
    editorPage->setContentEditable(false);

    connect(this->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(exposeToJavascript()));
}


void NWebView::editAlert() {
    isDirty =true;
}


void NWebView::exposeToJavascript() {
    page()->mainFrame()->addToJavaScriptWindowObject("jsbridge", this);
}



void NWebView::decryptText(QString id, QString text, QString hint) {
    /* Suppress unused */
    id=id;
    text=text;
    hint=hint;


    EnCrypt crypt;
    QString plainText;
/*
//    // First, try to decrypt with any keys we already have
    for (int i=0; i<global.passwordRemember.size(); i++) {
        QString key = global.passwordRemember[i].second;
        bool retVal = crypt.decrypt(text, plainText, key);
        if (retVal && plainText != "") {
            //slot = new String(Long.toString(l));
            //Global.passwordSafe.put(slot, Global.passwordRemember.get(i));
            //removeEncryption(id, plainText, false, slot);
            return;
        }
    }


    EnDecryptDialog dialog;
    dialog.hint.setText(hint);
    bool goodReturn = true;
    while (goodReturn && (plainText == "" || !dialog.okPressed)) {
        dialog.exec();
        if (!dialog.okPressed) {
            return;
        }
        QString pswd = dialog.password.text().trimmed();
        crypt.decrypt(text, plainText, pswd);
        if (plainText == "") {
            QMessageBox mb;
            mb.warning(this, tr("Incorrect Password"), tr("The password entered is not correct"));
        }
    }
//    Pair<String,String> passwordPair = new Pair<String,String>();
//    passwordPair.setFirst(dialog.getPassword());
//    passwordPair.setSecond(dialog.getHint());
//    Global.passwordSafe.put(slot, passwordPair);
//    removeEncryption(id, plainText, dialog.permanentlyDecrypt(), slot);
//    if (dialog.rememberPassword()) {
//        Pair<String, String> pair = new Pair<String,String>();
//        pair.setFirst(dialog.getPassword());
//        pair.setSecond(dialog.getHint());
//        Global.passwordRemember.add(pair);
//    }
*/
}
