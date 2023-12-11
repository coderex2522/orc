

#include "DataMask.hh"
#include "Adaptor.hh"

#if defined(__clang__)
  DIAGNOSTIC_IGNORE("-Wunused-parameter")
#endif

namespace orc {

DataMask::~DataMask() {}

class NullifyMask : public DataMask {
public:
    virtual ~NullifyMask() override {}

    void maskData(const ColumnVectorBatch& original,
                          ColumnVectorBatch& masked,
                          int start, int length) override;
};

void NullifyMask::maskData(const ColumnVectorBatch& original,
                           ColumnVectorBatch& masked,
                           int start, int length) {
    masked.hasNulls = true;
    for (int i = 0; i < length; ++i) {
        masked.notNull[start + i] = false;
    }
}


class StructMask : public DataMask {
public:
    StructMask(const std::vector<std::shared_ptr<DataMask>>& dataMasks);
    virtual ~StructMask() override {}

    void maskData(const ColumnVectorBatch& original,
                          ColumnVectorBatch& masked,
                          int start, int length) override;

private:
    std::vector<std::shared_ptr<DataMask>> children;
};

StructMask::StructMask(const std::vector<std::shared_ptr<DataMask>>& dataMasks)
    : children(dataMasks) {
}

void StructMask::maskData(const ColumnVectorBatch& original,
                               ColumnVectorBatch& masked,
                               int start, int length) {
    
}

class ListMask : public DataMask {
public:
    virtual ~ListMask() override {}

    void maskData(const ColumnVectorBatch& original,
                          ColumnVectorBatch& masked,
                          int start, int length) override;

private:
    std::vector<std::shared_ptr<DataMask>> children;
};

void ListMask::maskData(const ColumnVectorBatch& original,
                               ColumnVectorBatch& masked,
                               int start, int length) {
    masked.hasNulls = original.hasNulls;
    for (int i = 0; i < length; ++i) {
        masked.notNull[start + i] = original.notNull[start + i];
    }
}

class MapMask : public DataMask {
public:
    virtual ~MapMask() override {}

    void maskData(const ColumnVectorBatch& original,
                          ColumnVectorBatch& masked,
                          int start, int length) override;

private:
    std::vector<std::shared_ptr<DataMask>> children;
};

void MapMask::maskData(const ColumnVectorBatch& original,
                               ColumnVectorBatch& masked,
                               int start, int length) {
    masked.hasNulls = original.hasNulls;
    for (int i = 0; i < length; ++i) {
        masked.notNull[start + i] = original.notNull[start + i];
    }
}





}
