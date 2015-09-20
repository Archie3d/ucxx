#ifndef UCXX_IVARIANTSERIALIZER_H
#define UCXX_IVARIANTSERIALIZER_H

//
// Interface to a serializer based on variant data type
//

#include "Variant.h"

namespace ucxx {

class IVariantSerializer
{
public:
    virtual void pushValue(const Variant &value) = 0;
    virtual bool popValue(Variant &v) = 0;
    virtual ~IVariantSerializer() {}
};

} // namespace ucxx

#endif // UCXX_IVARIANTSERIALIZER_H
