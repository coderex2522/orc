#pragma once
#include "orc/Type.hh"
#include "orc/Vector.hh"

namespace orc {

  enum class DataMaskKind { NULLIFY, REDACT, SHA256 };

  class DataMask {
   public:
    virtual ~DataMask();

    virtual void maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                          int length) = 0;
  };

  class NullifyMask : public DataMask {
   public:
    NullifyMask() = default;

    virtual ~NullifyMask() override {}

    void maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                  int length) override;
  };

  class StructMask : public DataMask {
   public:
    StructMask(const std::vector<std::shared_ptr<DataMask>>& dataMasks);

    ~StructMask() override {}

    void maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                  int length) override;

   private:
    std::vector<std::shared_ptr<DataMask>> children;
  };

  class ListMask : public DataMask {
   public:
    ListMask(const std::vector<std::shared_ptr<DataMask>>& dataMasks);

    ~ListMask() override {}

    void maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                  int length) override;

   private:
    std::shared_ptr<DataMask> children;
  };

  class MapMask : public DataMask {
   public:
    MapMask(const std::vector<std::shared_ptr<DataMask>>& dataMasks);

    ~MapMask() override {}

    void maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                  int length) override;

   private:
    std::shared_ptr<DataMask> keyMask;
    std::shared_ptr<DataMask> valueMask;
  };

  class UnionMask : public DataMask {
   public:
    UnionMask(const std::vector<std::shared_ptr<DataMask>>& dataMasks);

    ~UnionMask() override {}

    void maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                  int length) override;

   private:
    std::vector<std::shared_ptr<DataMask>> children;
  };


}  // namespace orc
