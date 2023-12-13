#pragma once


#include "DataMask.hh"
#include "orc/Type.hh"

#include <map>
#include <memory>
namespace orc {
  class MaskFactory {
   public:
    virtual ~MaskFactory() {}

    virtual std::shared_ptr<DataMask> buildBooleanMask(const Type& type) = 0;
    virtual std::shared_ptr<DataMask> buildLongMask(const Type& type) = 0;
    virtual std::shared_ptr<DataMask> buildDoubleMask(const Type& type) = 0;
    virtual std::shared_ptr<DataMask> buildStringMask(const Type& type) = 0;
    virtual std::shared_ptr<DataMask> buildBinaryMask(const Type& type) = 0;
    virtual std::shared_ptr<DataMask> buildDateMask(const Type& type) = 0;
    virtual std::shared_ptr<DataMask> buildTimestampMask(const Type& type) = 0;

    virtual std::vector<std::shared_ptr<DataMask>> buildChildren(const Type& type);
    virtual std::shared_ptr<DataMask> buildUnionMask(const Type& type);
    virtual std::shared_ptr<DataMask> buildStructMask(const Type& type);
    virtual std::shared_ptr<DataMask> buildListMask(const Type& type);
    virtual std::shared_ptr<DataMask> buildMapMask(const Type& type);

    virtual std::shared_ptr<DataMask> build(const Type& type);
  };

  class RedactMaskFactory : public MaskFactory {
   public:
    RedactMaskFactory(const std::vector<std::string>& params);
    ~RedactMaskFactory() override = default;
    std::shared_ptr<DataMask> buildBooleanMask(const Type& type) override;
    std::shared_ptr<DataMask> buildLongMask(const Type& type) override;
    std::shared_ptr<DataMask> buildDoubleMask(const Type& type) override;
    std::shared_ptr<DataMask> buildStringMask(const Type& type) override;
    std::shared_ptr<DataMask> buildBinaryMask(const Type& type) override;
    std::shared_ptr<DataMask> buildDateMask(const Type& type) override;
    std::shared_ptr<DataMask> buildTimestampMask(const Type& type) override;
  
    int64_t maskInteger(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                             int length);
   private:
    std::map<int64_t, int64_t> unmaskIndexRanges;

  };

  class Sha256MaskFactory : public MaskFactory {
   public:
    ~Sha256MaskFactory() override {}
    std::shared_ptr<DataMask> buildBooleanMask(const Type& type) override;
    std::shared_ptr<DataMask> buildLongMask(const Type& type) override;
    std::shared_ptr<DataMask> buildDoubleMask(const Type& type) override;
    std::shared_ptr<DataMask> buildStringMask(const Type& type) override;
    std::shared_ptr<DataMask> buildBinaryMask(const Type& type) override;
    std::shared_ptr<DataMask> buildDateMask(const Type& type) override;
    std::shared_ptr<DataMask> buildTimestampMask(const Type& type) override;
  };

}  // namespace orc
