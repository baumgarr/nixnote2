#include "encrypt.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string.h>
#include <QString>


// BlowFish
//#define MY_CIPHER_MODE EVP_bf_cbc()		// Blowfish CBC mode
//#define MY_CIPHER_MODE EVP_bf_ecb()		// Blowfish ECB mode

// DES
//#define MY_CIPHER_MODE EVP_des_cbc()		// DES CBC mode
//#define MY_CIPHER_MODE EVP_des_ecb()		// DES ECB mode
//#define MY_CIPHER_MODE EVP_des_ede()		// DES EDE mode
//#define MY_CIPHER_MODE EVP_des_ede3()		// DES EDE3 mode

// RC2
#define MY_CIPHER_MODE EVP_rc2_cbc()		// RC2 CBC mode
//#define MY_CIPHER_MODE EVP_rc2_ecb()		// RC2 ECB mode

// RC4
//#define MY_CIPHER_MODE EVP_rc4()			// RC4 mode
//#define MY_CIPHER_MODE EVP_rc4_40()		// RC4 40 mode

EnCrypt::EnCrypt()
{
}

bool EnCrypt::encrypt(QString &input, QString &output, QString key) {
    //unsigned char iv[EVP_MAX_BLOCK_LENGTH];
    unsigned char out[output.length()];
    unsigned char keyChar[key.length()];

    //RAND_bytes(iv, EVP_MAX_IV_LENGTH);
    unsigned char iv[] = {0,1,2,3,4,5,6,7,8,9};

    int outlen;
    bool retVal = this->encrypt(input.toStdString().c_str(), out, &outlen, keyChar, iv);
    if (!retVal)
        return false;

    output = QString::fromAscii((const char *)out, outlen);
    return true;
}

bool EnCrypt::encrypt(const char *in, unsigned char *out, int *outlen, unsigned char *key, unsigned char *iv) {
    RAND_bytes(iv, EVP_MAX_IV_LENGTH);
    int buflen, tmplen;

    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);
    EVP_EncryptInit_ex(&ctx, MY_CIPHER_MODE, NULL, key, iv);

    if(!EVP_EncryptUpdate(&ctx, out, &buflen, (unsigned char*)in, strlen(in)))
    {
        return false;
    }

    if(!EVP_EncryptFinal_ex(&ctx, out + buflen, &tmplen))
    {
        return false;
    }

    buflen += tmplen;
    *outlen = buflen;
    EVP_CIPHER_CTX_cleanup(&ctx);

    return true;
}


bool EnCrypt::decrypt(QString &input, QString &output, QString &key) {
    unsigned char iv[] = {0,1,2,3,4,5,6,7,8,9};
    unsigned char out[input.length()];

    unsigned char in[input.length()];
    strcpy((char*)in, input.toAscii().data());

    unsigned char k[key.length()];
    strcpy((char*)k, key.toAscii().data());

    int len = input.length();
    bool retval = decrypt(in, out, len, k, iv);
    if (!retval)
        return false;

    output = QString::fromAscii((char*)out, len);
    return true;
}

bool EnCrypt::decrypt(const unsigned char *in, unsigned char *out, int inlen, unsigned char *key, unsigned char *iv)
{
    int buflen, tmplen;

    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);
    EVP_DecryptInit_ex(&ctx, MY_CIPHER_MODE, NULL, key, iv);

    if(!EVP_DecryptUpdate(&ctx, out, &buflen, in, inlen))
    {
        return false;
    }

    if(!EVP_DecryptFinal_ex(&ctx, out + buflen, &tmplen))
    {
        return false;
    }

    int decryptedLength = buflen + tmplen;
    out[decryptedLength] = '\0';

    EVP_CIPHER_CTX_cleanup(&ctx);

    return true;
}
