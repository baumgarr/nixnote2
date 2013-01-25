#include "encrypt.h"
#include <openssl/evp.h>
#include <QString>
#include <algorithm>
#include <stdexcept>



#define MY_CIPHER_MODE EVP_rc2_cbc()    // RC2 CBC mode

using namespace std;

EnCrypt::EnCrypt()
{

    QString d = "Data";
    QString k = "key";
    encrypt(d, k, 10);

}



QString EnCrypt::encrypt(QString text, QString passphrase, int keylen) {
    int outlen;
    unsigned char iv[] = {1,2,3,4,5,6,7,8};


    unsigned char *pEncryptedStr;
    pEncryptedStr = (unsigned char*)malloc(text.length()*2+1);
    unsigned char pDecryptedStr[1024];

    memset(pEncryptedStr, 0, text.length()*2+1);
    memset(pDecryptedStr, 0, sizeof(pDecryptedStr));
    unsigned char *v;

    do_encrypt(text.toStdString().c_str(), pEncryptedStr, &outlen, (unsigned char *)passphrase.toStdString().c_str(), iv);
    string rval(reinterpret_cast<const char *> (pEncryptedStr),
            sizeof (pEncryptedStr) / sizeof (pEncryptedStr[0]));

    string asHex = string_to_hex(rval);
    string asStr = hex_to_string(asHex);

    bool rv = do_decrypt((unsigned char *)asStr.c_str(), pDecryptedStr, outlen, (unsigned char*)passphrase.toStdString().c_str(), iv);

    int i=0;
    i++;
}


QString EnCrypt::decrypt(QString text, QString passphrase, int keylen) {

}



bool EnCrypt::do_encrypt(const char *in, unsigned char *out, int *outlen, unsigned char *key, unsigned char *iv) {
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

bool EnCrypt::do_decrypt(const unsigned char *in, unsigned char *out, int inlen, unsigned char *key, unsigned char *iv) {
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


string EnCrypt::string_to_hex(const std::string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}


string EnCrypt::hex_to_string(const std::string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();
    if (len & 1) throw std::invalid_argument("odd length");

    std::string output;
    output.reserve(len / 2);
    for (size_t i = 0; i < len; i += 2)
    {
        char a = input[i];
        const char* p = std::lower_bound(lut, lut + 16, a);
        if (*p != a) throw std::invalid_argument("not a hex digit");

        char b = input[i + 1];
        const char* q = std::lower_bound(lut, lut + 16, b);
        if (*q != b) throw std::invalid_argument("not a hex digit");

        output.push_back(((p - lut) << 4) | (q - lut));
    }
    return output;
}
