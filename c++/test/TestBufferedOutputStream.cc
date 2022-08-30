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

#include "wrap/orc-proto-wrapper.hh"
#include "wrap/gtest-wrapper.h"

#include "MemoryOutputStream.hh"
#include "EncryptionAlgorithm.hh"

namespace orc {
  TEST(BufferedOutputStream, block_aligned) {
    MemoryOutputStream memStream(1024);
    MemoryPool * pool = getDefaultPool();

    uint64_t capacity = 1000;
    uint64_t block = 10;
    WriterMetrics metrics;
    BufferedOutputStream bufStream(
            *pool, &memStream, capacity, block, &metrics);
    for (int i = 0; i < 100; ++i) {
      char * buf;
      int len;
      EXPECT_TRUE(bufStream.Next(reinterpret_cast<void **>(&buf), &len));
      EXPECT_EQ(10, len);
      for (int j = 0; j < 10; ++j) {
        buf[j] = static_cast<char>('a' + j);
      }
    }

    bufStream.flush();
    EXPECT_EQ(1000, memStream.getLength());
    for (int i = 0; i < 1000; ++i) {
      EXPECT_EQ(memStream.getData()[i], 'a' + i % 10);
    }
#if ENABLE_METRICS
    EXPECT_EQ(metrics.IOCount.load(), 1);
#endif
  }

  TEST(BufferedOutputStream, block_not_aligned) {
    MemoryOutputStream memStream(1024);
    MemoryPool * pool = getDefaultPool();

    uint64_t capacity = 20;
    uint64_t block = 10;
    WriterMetrics metrics;
    BufferedOutputStream bufStream(
            *pool, &memStream, capacity, block, &metrics);

    char * buf;
    int len;
    EXPECT_TRUE(bufStream.Next(reinterpret_cast<void **>(&buf), &len));
    EXPECT_EQ(10, len);

    for (int i = 0; i < 7; ++i) {
        buf[i] = static_cast<char>('a' + i);
    }

    bufStream.BackUp(3);
    bufStream.flush();

    EXPECT_EQ(7, memStream.getLength());
    for (int i = 0; i < 7; ++i) {
      EXPECT_EQ(memStream.getData()[i], 'a' + i);
    }

    EXPECT_TRUE(bufStream.Next(reinterpret_cast<void **>(&buf), &len));
    EXPECT_EQ(10, len);

    for (int i = 0; i < 5; ++i) {
        buf[i] = static_cast<char>('a' + i);
    }

    bufStream.BackUp(5);
    bufStream.flush();

    EXPECT_EQ(12, memStream.getLength());
    for (int i = 0; i < 7; ++i) {
      EXPECT_EQ(memStream.getData()[i], 'a' + i);
    }

    for (int i = 0; i < 5; ++i) {
     EXPECT_EQ(memStream.getData()[i + 7], 'a' + i);
    }
#if ENABLE_METRICS
    EXPECT_EQ(metrics.IOCount.load(), 2);
#endif
  }

  TEST(BufferedOutputStream, protobuff_serialization) {
    MemoryOutputStream memStream(1024);
    MemoryPool * pool = getDefaultPool();

    uint64_t capacity = 20;
    uint64_t block = 10;
    WriterMetrics metrics;
    BufferedOutputStream bufStream(
            *pool, &memStream, capacity, block, &metrics);

    proto::PostScript ps;
    ps.set_footerlength(197934);
    ps.set_compression(proto::ZLIB);
    ps.add_version(6);
    ps.add_version(20);
    ps.set_metadatalength(100);
    ps.set_writerversion(789);
    ps.set_magic("protobuff_serialization");

    EXPECT_TRUE(ps.SerializeToZeroCopyStream(&bufStream));
    bufStream.flush();
    EXPECT_EQ(ps.ByteSizeLong(), memStream.getLength());

    proto::PostScript ps2;
    ps2.ParseFromArray(
                       memStream.getData(),
                       static_cast<int>(memStream.getLength()));

    EXPECT_EQ(ps.footerlength(), ps2.footerlength());
    EXPECT_EQ(ps.compression(), ps2.compression());
    EXPECT_EQ(ps.version(0), ps2.version(0));
    EXPECT_EQ(ps.version(1), ps2.version(1));
    EXPECT_EQ(ps.metadatalength(), ps2.metadatalength());
    EXPECT_EQ(ps.writerversion(), ps2.writerversion());
    EXPECT_EQ(ps.magic(), ps2.magic());
  }

  TEST(BufferedOutputStream, testEncryption) {
    MemoryPool * pool = getDefaultPool();
    MemoryOutputStream memStream(1024);

    uint64_t capacity = 210;
    uint64_t block = 210;
    EncryptionOptions options;
    options.type = EncryptionAlgorithm::AES_CTR_128();
    options.key.resize(EncryptionAlgorithm::AES_CTR_128()->getKeyLength());
    options.iv.resize(EncryptionAlgorithm::AES_CTR_128()->getIvLength());
    for (int i = 0; i < 16; ++i) {
      options.key[i] = (char)i;
    }
    unsigned char* iv =
      reinterpret_cast<unsigned char*>(const_cast<char*>(options.iv.data()));
    CryptoUtils::modifyIvForStream(52, StreamKind_DATA, 18, iv);
    BufferedOutputStream bufStream(
            *pool, &memStream, capacity, block, nullptr, options);

    void* buffer = nullptr;
    int bufferSize = 0;
    bufStream.Next(&buffer, &bufferSize);
    EXPECT_EQ(bufferSize, 210);
    char* data = reinterpret_cast<char*>(buffer);
    for (int i = 0; i < bufferSize; ++i) {
      data[i] = (char)(i + 3);
    }
    bufStream.flush();
    EXPECT_EQ(memStream.getLength(), 210);
    int generated[210] = {
      0x82, 0x22, 0x52, 0xA8, 0x1C, 0xC7, 0xE7, 0xFE, // block 0
      0x3E, 0x51, 0xF5, 0x0E, 0x0E, 0x9B, 0x64, 0xB1,

      0xF6, 0x4D, 0x36, 0xA9, 0xD9, 0xD7, 0x55, 0xDE, // block 1
      0xCB, 0xD5, 0x62, 0x0E, 0x6D, 0xA6, 0x6B, 0x16,

      0x00, 0x0B, 0xE8, 0xBA, 0x9D, 0xDE, 0x78, 0xEC, // block 2
      0x73, 0x05, 0xF6, 0x1E, 0x76, 0xD7, 0x9B, 0x7A,

      0x47, 0xE9, 0x61, 0x90, 0x65, 0x8B, 0x54, 0xAC, // block 3
      0xF2, 0x3F, 0x67, 0xAE, 0x25, 0x63, 0x1D, 0x4B,

      0x41, 0x48, 0xC4, 0x15, 0x5F, 0x2A, 0x7F, 0x91, // block 4
      0x9A, 0x87, 0xA1, 0x09, 0xFF, 0x68, 0x68, 0xCC,

      0xC0, 0x80, 0x52, 0xD4, 0xA5, 0x07, 0x4B, 0x79, // block 5
      0xC7, 0x08, 0x46, 0x46, 0x8C, 0x74, 0x2C, 0x0D,

      0x9F, 0x55, 0x7E, 0xA7, 0x17, 0x47, 0x91, 0xFD, // block 6
      0x01, 0xD4, 0x24, 0x1F, 0x76, 0xA1, 0xDC, 0xC3,

      0xEA, 0x13, 0x4C, 0x29, 0xCA, 0x68, 0x1E, 0x4F, // block 7
      0x0D, 0x19, 0xE5, 0x09, 0xF9, 0xC5, 0xF4, 0x15,

      0x9A, 0xAD, 0xC4, 0xA1, 0x0F, 0x28, 0xD4, 0x3D, // block 8
      0x59, 0xF0, 0x68, 0xD3, 0xC4, 0x98, 0x74, 0x68,

      0x37, 0xA4, 0xF4, 0x7C, 0x02, 0xCE, 0xC6, 0xCA, // block 9
      0xA1, 0xF8, 0xC3, 0x8C, 0x7B, 0x72, 0x38, 0xD1,

      0xAA, 0x52, 0x90, 0xDE, 0x28, 0xA1, 0x53, 0x6E, // block a
      0xA6, 0x5C, 0xC0, 0x89, 0xC4, 0x21, 0x76, 0xC0,

      0x1F, 0xED, 0x0A, 0xF9, 0xA2, 0xA7, 0xC1, 0x8D, // block b
      0xA0, 0x92, 0x44, 0x4F, 0x60, 0x51, 0x7F, 0xD8,

      0x6D, 0x16, 0xAF, 0x46, 0x1C, 0x27, 0x20, 0x1C, // block c
      0x01, 0xBD, 0xC5, 0x0B, 0x62, 0x3F, 0xEF, 0xEE,

      0x37, 0xae                                      // block d
    };
    for (int i = 0; i < 210; ++i) {
      EXPECT_EQ(memStream.getData()[i], (char)generated[i] ^ data[i]);
    }
  }
}
