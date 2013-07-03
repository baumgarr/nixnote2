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
#include "botan/botan_all.h"
#include "global.h"
#include <boost/crc.hpp>
#include "crypto++/rc2.h"

extern Global global;



//#define MY_CIPHER_MODE EVP_rc2_cbc()    // RC2 CBC mode

using namespace std;
using namespace Botan;

EnCrypt::EnCrypt()
{
}



QString EnCrypt::decrypt(QString text, QString passphrase) {

    QCryptographicHash md5hash(QCryptographicHash::Md5);
    qDebug() << passphrase.toUtf8();
    md5hash.hash(passphrase.toUtf8(), QCryptographicHash::Md5);
    QByteArray hashResult = md5hash.result();

    SymmetricKey key((const byte*)hashResult.constData(), hashResult.size()); // encoded string
    // Turn the hex string back into cypher gibberish
//    QByteArray ba64;
//    ba64.append(text);
//    QByteArray ba = QByteArray::fromBase64(ba64);

    QByteArray ba = QByteArray::fromBase64("OI9WcCRoSLwSr6z4i2pvu9XsgMx0k8zxg1Tzk2bn5yXelooq/CVXJxywqgA9aKw2MERlFK2mXu9dHr1oJQ+YWflGVc58NeNWiE/JP8qlo6rzSs0RXFgGbDdOfSdlzgcS");

    // Separate out the CRC & the text
    try {
        // The algorithm we want is specified by a string
        Pipe pipe(get_cipher("RC2/ECB/NoPadding", key, DECRYPTION));
        byte* x;
        x = (byte*)malloc(ba.length());
        memcpy(x, ba.constData(), ba.length());
        pipe.process_msg(x,ba.length());
        SecureVector<byte> v = pipe.read_all();
        free(x);

        byte* x1;
        x1 = (byte*)malloc(v.size());
        memcpy(x1,v.begin(), v.size());
        QByteArray ba1;
        ba1.append((const char*)x1, v.size());
        free(x1);

        QString c1(ba1);
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
    result.process_bytes(ba.constData(), ba.size());
    qulonglong  crc = result.checksum();
    QByteArray rc;
    rc.setNum(crc,16);
    return rc.mid(0,4).toUpper();
}




