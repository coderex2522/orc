
#include "MaskFactory.hh"

namespace orc {

std::shared_ptr<DataMask> MaskFactory::build(const Type& type) {
    switch (type.getKind())
    {
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




}
