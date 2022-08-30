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

#ifndef ORC_ENCRYPTION_ALGORITHM_H
#define ORC_ENCRYPTION_ALGORITHM_H

#include <string>

namespace orc {

class EncryptionAlgorithm {
private:
    int keyLength;
    int ivLength;

public:
    static const EncryptionAlgorithm* AES_CTR_NOPADDING_128();
    static const EncryptionAlgorithm* AES_CTR_NOPADDING_256();

    EncryptionAlgorithm(int keyLength_, int ivLength_) :
                        keyLength(keyLength_), ivLength(ivLength_) {
    }
    /**
     * Get key length
     */
    uint32_t getKeyLength() const {
        return this->keyLength;
    }

    /**
     * Get iv length
     */
    uint32_t getIvLength() const {
        return this->ivLength;
    }
};


struct EncryptionOptions {
    const EncryptionAlgorithm* type;
    std::string key;
    std::string iv;

    EncryptionOptions() : type(nullptr) {
    }
};

}

#endif
