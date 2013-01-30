#include "encrypt.h"
#include <openssl/evp.h>
#include <QString>
#include <algorithm>
#include <stdexcept>
#include "botan/botan_all.h"
#include "global.h"
#include <boost/crc.hpp>

extern Global global;



#define MY_CIPHER_MODE EVP_rc2_cbc()    // RC2 CBC mode

using namespace std;
using namespace Botan;

EnCrypt::EnCrypt()
{

    QString d = "asd1jkl12345";
    QString k = "asd1jkl";
    QString enc = encrypt(d, k);
    QString dec = decrypt(enc, k);

}



QString EnCrypt::encrypt(QString text, QString passphrase) {
    QByteArray xkey2(passphrase.toUtf8());
    SymmetricKey key((const byte*)xkey2.data(), xkey2.length()); // encoded string

    // Pad the key & passphrase
    QByteArray textArray = text.toUtf8();

    int align8 = (textArray.length()+4)%8;
    int paddingNeeded = 8-align8;
    int size = textArray.length();
    textArray.resize(paddingNeeded+size);
    for (int i=0; i<paddingNeeded; i++)
        textArray[size+i] = 0;


    // now calcualate the CRC
    QByteArray crc;
    crc = CRC32(textArray);
    if (crc.length()!=4) {
        QLOG_ERROR() << "Invalid CRC returned";
        return "";
    }
    QByteArray total = crc+textArray;


    // The algorithm we want is specified by a string
    Pipe pipe(get_cipher("RC2/ECB/NoPadding", key, ENCRYPTION));

    pipe.process_msg((const byte*)total.data(), total.size());

    string c1 = pipe.read_all_as_string();
    QByteArray result;
    result.append(QString::fromStdString(c1));
    result = result.toBase64();
    return QString(result);
}


QString EnCrypt::decrypt(QString text, QString passphrase) {
    QByteArray xkey2;
    xkey2.append(passphrase.toUtf8());


    SymmetricKey key((const byte*)xkey2.data(), xkey2.length()); // encoded string

    // Turn the hex string back into cypher gibberish
    QByteArray ba64;
    ba64.append(text);
    QByteArray ba = QByteArray::fromBase64(ba64);

    ba = QByteArray::fromBase64("OI9WcCRoSLwSr6z4i2pvu9XsgMx0k8zxg1Tzk2bn5yXelooq/CVXJxywqgA9aKw2MERlFK2mXu9dHr1oJQ+YWflGVc58NeNWiE/JP8qlo6rzSs0RXFgGbDdOfSdlzgcS");

    // Separate out the CRC & the text
    try {
        // The algorithm we want is specified by a string
        Pipe pipe(get_cipher("RC2/ECB/NoPadding", key, DECRYPTION));
        string stringMsg;
        stringMsg.append(ba.data(), ba.length());
        pipe.process_msg(stringMsg);
        QString c1 = QString::fromStdString(pipe.read_all_as_string());
        QString msgCrc = c1.mid(0,4);
        QString msg = c1.mid(4);
        ba.clear();
        ba.append(msg);
        QByteArray crc = CRC32(ba);
        if (crc  != msgCrc)
            return "";

        // Trim off trailing blanks
        int cnt = 0;
        bool finished = false;
        for (int i=msg.length()-1; i>=0 && !finished; i--) {
            if (msg[i] == 0)
                cnt++;
            else
                finished=true;
        }
        msg = msg.mid(0,msg.length()-cnt);
        return msg;


    } catch (Exception e) {
        QLOG_DEBUG() << e.what();
        return "";
    }
}




QByteArray EnCrypt::CRC32(QByteArray ba) {
    boost::crc_32_type result;
    result.process_bytes(ba.data(), ba.size());
    qulonglong  crc = result.checksum();
    QByteArray rc;
    rc.setNum(crc,16);
    return rc.mid(0,4).toUpper();
}
