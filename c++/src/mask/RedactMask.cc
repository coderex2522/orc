#include "RedactMask.hh"

#include "Adaptor.hh"

#if defined(__clang__)
DIAGNOSTIC_IGNORE("-Wunused-parameter")
DIAGNOSTIC_IGNORE("-Wswitch-enum")
#endif

namespace orc {
    RedactLongMask::RedactLongMask(const Type& type) {
    switch (type.getKind()) {
    case BYTE:
      maskValue = 0xff;
      break;
    case SHORT:
      maskValue = 0xffff;
      break;
    case INT:
      maskValue = 0xffffffff;
      break;
    case LONG:
    default:
      maskValue = -1;
      break;
    }
  }

  void RedactLongMask::maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked,
                                int start, int length) {
    const LongVectorBatch& source = dynamic_cast<const LongVectorBatch&>(original);
    LongVectorBatch& target = dynamic_cast<LongVectorBatch&>(masked);
    for (int i = 0; i < length; ++i) {
      target.data[start + i] = source.data[start + i] & maskValue;
    }
  }
}
