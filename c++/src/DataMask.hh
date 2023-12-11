
#include "orc/Vector.hh"

namespace orc {

class DataMask {
public:
    virtual ~DataMask();

    virtual void maskData(const ColumnVectorBatch& original,
                          ColumnVectorBatch& masked,
                          int start, int length) = 0;
};

}
