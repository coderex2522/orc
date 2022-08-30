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

#ifndef ORC_ENCRYPTION_ALGORITHM_IMPL_H
#define ORC_ENCRYPTION_ALGORITHM_IMPL_H

#include "orc/EncryptionAlgorithm.hh"
#include "orc/Common.hh"

#include <memory>
#include <stdint.h>

namespace orc {

class Cipher {
public:
    virtual ~Cipher();

    virtual void init() = 0;

    virtual void encrypt(const DataBuffer<char>& input,
                         DataBuffer<char>& encryptedOutput) = 0;

    static std::unique_ptr<Cipher> createInstance(
                         const EncryptionOptions& options);
};

class CryptoUtils {
public:
  static const int AES_CTR_128_KEY_LENGTH = 16;
  static const int AES_CTR_256_KEY_LENGTH = 32;
  static const int IV_LENGTH = 16;

  static const int COLUMN_ID_LENGTH = 3;
  static const int KIND_LENGTH = 2;
  static const int STRIPE_ID_LENGTH = 3;
  static const int MIN_COUNT_BYTES = 8;

  static const uint64_t MAX_STRIPE = 0xffffff;
  static const uint64_t MAX_COLUMN = 0xffffff;
  static const int MAX_KIND = 0xffff;

  static void clearCounter(unsigned char* iv); 

  static void modifyIvForStripe(uint64_t stripeIndex, unsigned char* iv);

  static void modifyIvForStream(uint64_t columnId,
                                StreamKind kind,
                                uint64_t stripeIndex,
                                unsigned char* iv);
};

}

#endif
