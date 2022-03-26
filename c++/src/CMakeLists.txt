# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAGS} ${WARN_FLAGS}")

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${LIBHDFSPP_INCLUDE_DIR}
  )

add_custom_command(OUTPUT orc_proto.pb.h orc_proto.pb.cc
   COMMAND ${PROTOBUF_EXECUTABLE}
        -I ${PROJECT_SOURCE_DIR}/proto
        --cpp_out="${CMAKE_CURRENT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/proto/orc_proto.proto"
)

set(SOURCE_FILES
  orc_proto.pb.h
  io/InputStream.cc
  io/OutputStream.cc
  sargs/ExpressionTree.cc
  sargs/Literal.cc
  sargs/PredicateLeaf.cc
  sargs/SargsApplier.cc
  sargs/SearchArgument.cc
  sargs/TruthValue.cc
  wrap/orc-proto-wrapper.cc
  Adaptor.cc
  BloomFilter.cc
  ByteRLE.cc
  ColumnPrinter.cc
  ColumnReader.cc
  ColumnWriter.cc
  Common.cc
  Compression.cc
  Exceptions.cc
  Int128.cc
  LzoDecompressor.cc
  MemoryPool.cc
  Murmur3.cc
  OrcFile.cc
  Reader.cc
  RLEv1.cc
  RLEV2Util.cc
  RleDecoderV2.cc
  RleEncoderV2.cc
  RLE.cc
  Statistics.cc
  StripeStream.cc
  Timezone.cc
  TypeImpl.cc
  Vector.cc
  Writer.cc)

if(BUILD_LIBHDFSPP)
  set(SOURCE_FILES ${SOURCE_FILES} OrcHdfsFile.cc)
  add_definitions(-DBUILD_LIBHDFSPP)
endif(BUILD_LIBHDFSPP)

add_library (orc STATIC ${SOURCE_FILES})

target_link_libraries (orc
  orc::protobuf
  orc::zlib
  orc::snappy
  orc::lz4
  orc::zstd
  ${LIBHDFSPP_LIBRARIES}
  )

install(TARGETS orc DESTINATION lib)
