/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2013 Randy Baumgarte

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

#include "encrypt.h"
#include <openssl/evp.h>
#include <QString>
#include <algorithm>
#include <stdexcept>
#include "global.h"
#include <boost/crc.hpp>
//#include "crypto++/rc2.h"
//#include <mcrypt.h>
#include <QCryptographicHash>

extern Global global;



//#define MY_CIPHER_MODE EVP_rc2_cbc()    // RC2 CBC mode

using namespace std;

EnCrypt::EnCrypt()
{
}


int EnCrypt::decrypt(QString &result, QString text, QString passphrase) {
    return decrypt(result, text, passphrase, "RC2", 64);
}

int EnCrypt::decrypt(QString &result, QString text, QString passphrase, QString cipher, int length) {

    if (cipher == "RC2")
        return this->runner(result, text, passphrase, "decrypt-rc2", length);
    return this->Invaid_Method;
}


int EnCrypt::encrypt(QString &result, QString text, QString passphrase) {
    return encrypt(result, text, passphrase, "RC2", 64);
}

int EnCrypt::encrypt(QString &result, QString text, QString passphrase, QString cipher, int length) {

    if (cipher == "RC2")
        return this->runner(result, text, passphrase, "encrypt-rc2", length);
    return this->Invaid_Method;
}


int EnCrypt::runner(QString &result, QString text, QString passphrase, QString method,  int keylen) {

    // Run the Java program to decrypt the text.  This is an extremely ugly
    // hack, but I haven't been able to get anything else to work.
    QString jar = global.fileManager.getJavaDirPath("") + "crypto.jar";

    QProcess javaProcess;
    QStringList args;
    args.append("-jar");
    args.append(jar);
    args.append(method);
    args.append(passphrase);
    args.append(QString::number(keylen));
    args.append(text);
    javaProcess.start("java", args);

    result.clear();
    javaProcess.waitForFinished();
    int rc = javaProcess.exitCode();
    QLOG_DEBUG() << "Return Code: " << rc;
    if (rc == 0)
        result.append(javaProcess.readAllStandardOutput());

    return rc;

//    tidyProcess.start("tidy -raw -asxhtml -q -m -u -utf8 ", QIODevice::ReadWrite|QIODevice::Unbuffered);
//    QLOG_DEBUG() << "Starting tidy " << tidyProcess.waitForStarted();
//    tidyProcess.waitForStarted();
//    tidyProcess.write(content);
//    tidyProcess.closeWriteChannel();
//    tidyProcess.waitForFinished();
//    QLOG_DEBUG() << "Stopping tidy " << tidyProcess.waitForFinished() << " Return Code: " << tidyProcess.state();
//    QLOG_DEBUG() << "Tidy Errors:" << tidyProcess.readAllStandardError();
//    content.clear();
//    content.append(tidyProcess.readAllStandardOutput());
//    if (content == "") {
//        formattingError = true;
//        return "";
//    }


    // Below is a botan test
//    QByteArray hash = QCryptographicHash::hash(passphrase.toUtf8(),QCryptographicHash::Md5);
//    SymmetricKey key;
//    key.change(QString(hash.toHex()).toStdString());
//    qDebug() << "Key:" << QString::fromStdString(key.as_string());

//    try {
//        Keyed_Filter *cipher = get_cipher("RC2/ECB/NoPadding", DECRYPTION);
//        cipher->set_key(key);
//        Pipe pipe(new Base64_Decoder, cipher);
//        byte *in = (byte*)text.data();
//        QLOG_DEBUG() << text.length();
//        pipe.process_msg(in, text.length());
//        SecureVector<byte> sv =  pipe.read_all();
//        QByteArray x1 = QByteArray::fromRawData((const char*)sv.begin(), sv.size());
//        QLOG_DEBUG() << x1;
//        QLOG_DEBUG() << x1.toHex();
//        QLOG_DEBUG() << x1.toBase64();
//        QLOG_DEBUG() << QByteArray::fromBase64(x1);
//        QLOG_DEBUG() << "ptr";
//    } catch(std::exception& ex) {
//        QLOG_DEBUG() << "Decrypt failed: " << QString(ex.what());
//    }

//    return "";

// Below is an mcrypt trial
//    QLOG_DEBUG() << text;
//    QLOG_DEBUG() << hash.toHex();
//    MCRYPT td;
//    int i;
//    void *key;
//    char *block_buffer;
//    int keysize=hash.length();
//    int blocksize;
//    key = calloc(1, keysize);
//    memcpy(key, hash.data(), hash.length());
//    char cypher[] = "rc2";
//    char block[] = "ecb";

//    td = mcrypt_module_open(cypher, NULL, block, NULL);
//    if (td == MCRYPT_FAILED)
//        return "";

//      QByteArray ba;
//      ba.append(text);
//      ba = QByteArray::fromBase64(ba);
//      blocksize = mcrypt_enc_get_block_size(td);
//      block_buffer = (char *)malloc(ba.size()*blocksize);
//      memset(block_buffer, '\0', ba.size()*blocksize);
//      memcpy(block_buffer, ba.data(), ba.length());
//      qDebug() << block_buffer;

//      i=mcrypt_generic_init(td, key, keysize, NULL);
//      if (i<0) {
//         mcrypt_perror(i);
//         return "";
//      }
//      //mcrypt_generic (td, &block_buffer, 1);
//      int rc = mdecrypt_generic(td, block_buffer, ba.length());
//      QLOG_DEBUG() << rc;

//    /* Deinit the encryption thread, and unload the module */
//      mcrypt_generic_end(td);
//      QByteArray ba3;
//      ba3.append(block_buffer, ba.size()*blocksize);
//      QLOG_DEBUG() << ba3.toHex();
//      QLOG_DEBUG() << QByteArray::fromBase64(ba3);

//    return "";
}


QByteArray EnCrypt::CRC32(QByteArray ba) {
    boost::crc_32_type result;
    result.process_bytes(ba.constData(), ba.size());
    qulonglong  crc = result.checksum();
    QByteArray rc;
    rc.setNum(crc,16);
    return rc.mid(0,4).toUpper();
}




