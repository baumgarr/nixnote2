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

#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <QString>
#include <string>

using namespace std;

class EnCrypt
{
private:
    bool do_encrypt(const char *in, unsigned char *out, int *outlen, unsigned char *key, unsigned char *iv);
    bool do_decrypt(const unsigned char *in, unsigned char *out, int inlen, unsigned char *key, unsigned char *iv);
    string string_to_hex(const std::string& input);
    string hex_to_string(const std::string& input);
    int runner(QString &result, QString text, QString passphrase, QString method,  int keylen);

public:
    EnCrypt();
    enum CryptoResults {
        Java_Not_Found = 255,
        Invalid_Arguments = 16,
        Invaid_Method = 14,
        Invalid_Key = 4
    };

    int encrypt(QString&result, QString text, QString passphrase, QString cipher, int length);
    int encrypt(QString &result, QString text, QString passphrase);
    int decrypt(QString &result, QString text, QString passphrase, QString cipher, int length);
    int decrypt(QString &result, QString text, QString passphrase);
    //QByteArray CRC32(QByteArray ba);
};

#endif // ENCRYPT_H
