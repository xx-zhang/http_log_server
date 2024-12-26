#ifndef AES_CIPHER_H
#define AES_CIPHER_H

#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <string>
#include <vector>
#include <iostream>

class AESCipher {
public:
    AESCipher(const std::string &key, const std::string &iv);

    std::string Encrypt(const std::string &plaintext);
    std::string Decrypt(const std::string &encoded_ciphertext);

private:
    std::string key_;
    std::string iv_;

    std::string base64_encode(const unsigned char *input, int length);
    std::vector<unsigned char> base64_decode(const std::string &input);
};

#endif // AES_CIPHER_H
