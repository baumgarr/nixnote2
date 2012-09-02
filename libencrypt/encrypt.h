#ifndef ENCRYPT_H
#define ENCRYPT_H

#include "encrypt_global.h"

class ENCRYPTSHARED_EXPORT EnCrypt {
public:
    EnCrypt();
    bool encrypt(QString &input, QString &output, QString key);
    bool encrypt(const char *in, unsigned char *out, int *outlen, unsigned char *key, unsigned char *iv);

    bool decrypt(QString &input, QString &output, QString &key);
    bool decrypt(const unsigned char *in, unsigned char *out, int inlen, unsigned char *key, unsigned char *iv);
};

#endif // ENCRYPT_H
