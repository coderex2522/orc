
#include "MaskFactory.hh"

#include "Adaptor.hh"

#if defined(__clang__)
DIAGNOSTIC_IGNORE("-Wunused-parameter")
DIAGNOSTIC_IGNORE("-Wswitch-enum")
#endif

namespace orc {

  std::shared_ptr<DataMask> MaskFactory::build(const Type& type) {
    switch (type.getKind()) {
      case BOOLEAN:
        return buildBooleanMask(type);
      case BYTE:
      case SHORT:
      case INT:
      case LONG:
        return buildLongMask(type);
      case FLOAT:
      case DOUBLE:
        return buildDoubleMask(type);
      case STRING:
      case CHAR:
      case VARCHAR:
        return buildStringMask(type);
      case TIMESTAMP:
      case TIMESTAMP_INSTANT:
        return buildTimestampMask(type);
      case DATE:
        return buildDateMask(type);
      case BINARY:
        return buildBinaryMask(type);
      case UNION:
        return buildUnionMask(type);
      case STRUCT:
        return buildStructMask(type);
      case LIST:
        return buildListMask(type);
      case MAP:
        return buildMapMask(type);
      case DECIMAL:
      default:
        throw std::runtime_error("Unhandled type " + type.toString());
    }
  }

  std::vector<std::shared_ptr<DataMask>> MaskFactory::buildChildren(const Type& type) {
    std::vector<std::shared_ptr<DataMask>> result;
    for (size_t i = 0; i < type.getSubtypeCount(); ++i) {
      result.push_back(build(*type.getSubtype(i)));
    }
    return result;
  }

  std::shared_ptr<DataMask> MaskFactory::buildStructMask(const Type& type) {
    return std::make_shared<StructMask>(buildChildren(type));
  }

  std::shared_ptr<DataMask> MaskFactory::buildListMask(const Type& type) {
    return std::make_shared<ListMask>(buildChildren(type));
  }

  std::shared_ptr<DataMask> MaskFactory::buildMapMask(const Type& type) {
    return std::make_shared<MapMask>(buildChildren(type));
  }

  std::shared_ptr<DataMask> MaskFactory::buildUnionMask(const Type& type) {
    return std::make_shared<UnionMask>(buildChildren(type));
  }

  RedactMaskFactory::RedactMaskFactory(const std::vector<std::string>& params) {
    
  }

  std::shared_ptr<DataMask> RedactMaskFactory::buildBooleanMask(const Type& type) {
    return nullptr;
  }

  std::shared_ptr<DataMask> RedactMaskFactory::buildLongMask(const Type& type) {
    return nullptr;
  }

  std::shared_ptr<DataMask> RedactMaskFactory::buildDoubleMask(const Type& type) {
    return nullptr;
  }

  std::shared_ptr<DataMask> RedactMaskFactory::buildStringMask(const Type& type) {
    return nullptr;
  }

  std::shared_ptr<DataMask> RedactMaskFactory::buildBinaryMask(const Type& type) {
    return nullptr;
  }

  std::shared_ptr<DataMask> RedactMaskFactory::buildDateMask(const Type& type) {
    return nullptr;
  }

  std::shared_ptr<DataMask> RedactMaskFactory::buildTimestampMask(const Type& type) {
    return nullptr;
  }



  std::shared_ptr<DataMask> Sha256MaskFactory::buildBooleanMask(const Type& type) {
    return std::make_shared<NullifyMask>();
  }

  std::shared_ptr<DataMask> Sha256MaskFactory::buildLongMask(const Type& type) {
    return std::make_shared<NullifyMask>();
  }

  std::shared_ptr<DataMask> Sha256MaskFactory::buildDoubleMask(const Type& type) {
    return std::make_shared<NullifyMask>();
  }

  class StringMask : public DataMask {
    public:
      StringMask(const Type& type) {
      }
      virtual ~StringMask() override {}
      void maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                    int length) override;
  };

  void StringMask::maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                            int length) {
    // TODO
  }

  std::shared_ptr<DataMask> Sha256MaskFactory::buildStringMask(const Type& type) {
    return std::make_shared<StringMask>(type);
  }

  class BinaryMask : public DataMask {
    public:
      BinaryMask(const Type& type) {
      }
      virtual ~BinaryMask() override {}
      void maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                    int length) override;
  };

  void BinaryMask::maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                            int length) {
    // TODO
  }

  std::shared_ptr<DataMask> Sha256MaskFactory::buildBinaryMask(const Type& type) {
    return std::make_shared<BinaryMask>(type);
  }

  std::shared_ptr<DataMask> Sha256MaskFactory::buildDateMask(const Type& type) {
    return std::make_shared<NullifyMask>();
  }

  std::shared_ptr<DataMask> Sha256MaskFactory::buildTimestampMask(const Type& type) {
    return std::make_shared<NullifyMask>();
  }
}  // namespace orc
