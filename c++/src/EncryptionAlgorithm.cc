/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "EncryptionAlgorithm.hh"
#include "orc/Exceptions.hh"

#include <openssl/aes.h>
#include <openssl/evp.h>

#include <cstring>
#include <iostream>
#include <stdexcept>

namespace orc {

const EncryptionAlgorithm* EncryptionAlgorithm::AES_CTR_128() {
  static EncryptionAlgorithm encryptionAlgorithm(
      CryptoUtils::AES_CTR_128_KEY_LENGTH,
      CryptoUtils::IV_LENGTH);
  return &encryptionAlgorithm;
}

const EncryptionAlgorithm* EncryptionAlgorithm::AES_CTR_256() {
  static EncryptionAlgorithm encryptionAlgorithm(
      CryptoUtils::AES_CTR_256_KEY_LENGTH,
      CryptoUtils::IV_LENGTH);
  return &encryptionAlgorithm;
}

std::string CryptoUtils::getErrorString() {
  // void ERR_print_errors_cb(
  // int (*cb)(const char *str, size_t len, void *u), void *u)
  return "hello";
}

void CryptoUtils::clearCounter(unsigned char* iv) {
  int offset = COLUMN_ID_LENGTH + KIND_LENGTH + STRIPE_ID_LENGTH;
  memset(iv + offset, 0, IV_LENGTH - offset);
}

void CryptoUtils::modifyIvForStripe(uint64_t stripeIndex,
                                    unsigned char* iv) {
  if (stripeIndex < 1 || stripeIndex > MAX_STRIPE) {
    throw std::logic_error("stripe index out of range");
  }
  iv[COLUMN_ID_LENGTH + KIND_LENGTH] = (unsigned char) (stripeIndex & 0xff0000);
  iv[COLUMN_ID_LENGTH + KIND_LENGTH + 1] = (unsigned char) (stripeIndex & 0xff00);
  iv[COLUMN_ID_LENGTH + KIND_LENGTH + 2] = (unsigned char) (stripeIndex & 0xff);
  clearCounter(iv);
}

void CryptoUtils::modifyIvForStream(uint64_t columnId,
                                    StreamKind kind,
                                    uint64_t stripeIndex,
                                    unsigned char* iv) {
  int k = static_cast<int>(kind);
  iv[0] = (unsigned char) (columnId & 0xff0000);
  iv[1] = (unsigned char) (columnId & 0xff00);
  iv[2] = (unsigned char) (columnId & 0xff);
  iv[COLUMN_ID_LENGTH] = (unsigned char) (k & 0xff00);
  iv[COLUMN_ID_LENGTH + 1] = (unsigned char) (k & 0xff);
  modifyIvForStripe(stripeIndex, iv);
}

Cipher::~Cipher() {
  // PASS
}

class AesCtr128Cipher : public Cipher {
public:
  AesCtr128Cipher(const std::string& key, const std::string& iv);
  ~AesCtr128Cipher();

  void init() override;
  void encrypt(const DataBuffer<char>& input,
               DataBuffer<char>& encryptedOutput) override;

private:
  EVP_CIPHER_CTX *ctx;
  unsigned char key[CryptoUtils::AES_CTR_128_KEY_LENGTH];
  unsigned char iv[CryptoUtils::IV_LENGTH];
};

AesCtr128Cipher::AesCtr128Cipher(const std::string& key_,
                                 const std::string& iv_) {
  memcpy(key, key_.data(), CryptoUtils::AES_CTR_128_KEY_LENGTH);
  memcpy(iv, iv_.data(), CryptoUtils::IV_LENGTH);
}

void AesCtr128Cipher::init() {
  ctx = EVP_CIPHER_CTX_new();
  if (ctx == nullptr) {
    throw std::runtime_error("AES_CTR_128 ctx create error");
  }
  if(!EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, iv)) {
    throw std::runtime_error("AES_CTR_128 init error");
  }
  /* Disable padding */
  EVP_CIPHER_CTX_set_padding(ctx, 0);
}

AesCtr128Cipher::~AesCtr128Cipher() {
  // free the cipher ctx
  EVP_CIPHER_CTX_free(ctx);
}

void AesCtr128Cipher::encrypt(const DataBuffer<char>& input,
                              DataBuffer<char>& encryptedOutput) {
  if (input.size() == 0) {
    return;
  }
  encryptedOutput.resize(input.size());
  const unsigned char* inputPtr =
    reinterpret_cast<const unsigned char*>(input.data());
  unsigned char* outputPtr =
    reinterpret_cast<unsigned char*>(encryptedOutput.data()); 
  int outLength = 0;
  if (!EVP_EncryptUpdate(ctx, outputPtr, &outLength, inputPtr, (int)input.size())) {
    EVP_CIPHER_CTX_free(ctx);
    throw std::runtime_error("AES_CTR_128 encrypt data error");
  }
  int encryptLength = outLength;
  if(1 != EVP_EncryptFinal_ex(ctx, outputPtr + outLength, &outLength)) {
    EVP_CIPHER_CTX_free(ctx);
    throw std::runtime_error("AES_CTR_128 final encrypt error");
  }
  encryptLength += outLength;
}

std::unique_ptr<Cipher> Cipher::createInstance(const EncryptionOptions& options) {
  if (options.type == EncryptionAlgorithm::AES_CTR_128()) {
    if (options.key.size() !=
        EncryptionAlgorithm::AES_CTR_128()->getKeyLength()) {
      throw std::logic_error("AES_CTR_128 key length invalid.");
    }
    if (options.iv.size() !=
        EncryptionAlgorithm::AES_CTR_128()->getIvLength()) {
      throw std::logic_error("AES_CTR_128 iv length invalid.");
    }
    std::unique_ptr<Cipher> cipher(new AesCtr128Cipher(options.key, options.iv));
    cipher->init();
    return std::move(cipher);
  } else if (options.type == EncryptionAlgorithm::AES_CTR_256()) {
    throw NotImplementedYet("EncryptionAlgorithm is not supported yet "
                            "for creating cipher."); 
  }
  return nullptr;
}

}
