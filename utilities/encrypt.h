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


public:
    EnCrypt();
    QString encrypt(QString text, QString passphrase, int keylen);
    QString decrypt(QString text, QString passphrase, int keylen);
};

#endif // ENCRYPT_H
