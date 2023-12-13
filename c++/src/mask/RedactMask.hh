#pragma once

#include "DataMask.hh"

namespace orc {
    class RedactLongMask : public DataMask {
    public:
      RedactLongMask(const Type& type);
      virtual ~RedactLongMask() override {}
      void maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                    int length) override;
    private:
      int64_t maskValue;
  };
}
