

#include "DataMask.hh"
#include "Adaptor.hh"
#include "MaskFactory.hh"

#include <memory>

#if defined(__clang__)
DIAGNOSTIC_IGNORE("-Wunused-parameter")
DIAGNOSTIC_IGNORE("-Wswitch-enum")
#endif

namespace orc {

  DataMask::~DataMask() {}

  void NullifyMask::maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked,
                             int start, int length) {
    masked.hasNulls = true;
    for (int i = 0; i < length; ++i) {
      masked.notNull[start + i] = false;
    }
  }

  StructMask::StructMask(const std::vector<std::shared_ptr<DataMask>>& dataMasks)
      : children(dataMasks) {}

  void StructMask::maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                            int length) {
    const StructVectorBatch& source = dynamic_cast<const StructVectorBatch&>(original);
    StructVectorBatch& target = dynamic_cast<StructVectorBatch&>(masked);
    for (size_t i = 0; i < children.size(); ++i) {
      children[i]->maskData(*source.fields[i], *target.fields[i], start, length);
    }
  }

  ListMask::ListMask(const std::vector<std::shared_ptr<DataMask>>& dataMasks) {
    children = dataMasks[0];
  }

  void ListMask::maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                          int length) {
    const ListVectorBatch& source = dynamic_cast<const ListVectorBatch&>(original);
    ListVectorBatch& target = dynamic_cast<ListVectorBatch&>(masked);
    children->maskData(*source.elements, *target.elements, source.offsets[start],
                       source.offsets[start + length]);
  }

  MapMask::MapMask(const std::vector<std::shared_ptr<DataMask>>& dataMasks) {
    keyMask = dataMasks[0];
    valueMask = dataMasks[1];
  }

  void MapMask::maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                         int length) {
    const MapVectorBatch& source = dynamic_cast<const MapVectorBatch&>(original);
    MapVectorBatch& target = dynamic_cast<MapVectorBatch&>(masked);
    keyMask->maskData(*source.keys, *target.keys, source.offsets[start],
                      source.offsets[start + length]);
    valueMask->maskData(*source.elements, *target.elements, source.offsets[start],
                        source.offsets[start + length]);
  }

  UnionMask::UnionMask(const std::vector<std::shared_ptr<DataMask>>& dataMasks)
      : children(dataMasks) {}

  void UnionMask::maskData(const ColumnVectorBatch& original, ColumnVectorBatch& masked, int start,
                           int length) {
    // TODO: implement
  }
}  // namespace orc
