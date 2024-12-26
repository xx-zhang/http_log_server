#include "AESCipher.h"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <iostream>

AESCipher::AESCipher(const std::string &key, const std::string &iv) :
    key_(key), iv_(iv) {
}

std::string AESCipher::Encrypt(const std::string &plaintext) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create context");

    int len;
    int ciphertext_len;
    std::vector<unsigned char> ciphertext(plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char *>(key_.c_str()), reinterpret_cast<const unsigned char *>(iv_.c_str()))) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptInit failed");
    }

    if (1 != EVP_EncryptUpdate(ctx, ciphertext.data(), &len, reinterpret_cast<const unsigned char *>(plaintext.c_str()), plaintext.size())) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptUpdate failed");
    }
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptFinal failed");
    }
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(ciphertext_len);

    return base64_encode(ciphertext.data(), ciphertext_len);
}

std::string AESCipher::Decrypt(const std::string &encoded_ciphertext) {
    int ciphertext_len;
    std::vector<unsigned char> ciphertext = base64_decode(encoded_ciphertext);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create context");

    std::vector<unsigned char> plaintext(ciphertext.size());
    int len;

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char *>(key_.c_str()), reinterpret_cast<const unsigned char *>(iv_.c_str()))) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptInit failed");
    }

    if (1 != EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext_len)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptUpdate failed");
    }
    int plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptFinal failed");
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    plaintext.resize(plaintext_len);

    return std::string(plaintext.begin(), plaintext.end());
}

std::string AESCipher::base64_encode(const unsigned char *input, int length) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;
    std::string encoded;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, input, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    encoded.assign(bufferPtr->data, bufferPtr->length);

    BIO_free_all(bio);
    return encoded;
}

std::vector<unsigned char> AESCipher::base64_decode(const std::string &input) {
    BIO *bio, *b64;
    int decodeLen = input.size();
    int len = 0;
    std::vector<unsigned char> buffer(decodeLen);

    bio = BIO_new_mem_buf(input.data(), -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    len = BIO_read(bio, buffer.data(), decodeLen);
    buffer.resize(len);

    BIO_free_all(bio);
    return buffer;
}
