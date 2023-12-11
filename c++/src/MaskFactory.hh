

#include "DataMask.hh"
#include "orc/Type.hh"
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


    virtual std::shared_ptr<DataMask> buildChildren(const Type& type);
    virtual std::shared_ptr<DataMask> buildUnionMask(const Type& type);
    virtual std::shared_ptr<DataMask> buildStructMask(const Type& type);
    virtual std::shared_ptr<DataMask> buildListMask(const Type& type);
    virtual std::shared_ptr<DataMask> buildMapMask(const Type& type);

    virtual std::shared_ptr<DataMask> build(const Type& type);
};


}
