#ifndef UCXX_BYTEARRAYSERIALIZER_H
#define UCXX_BYTEARRAYSERIALIZER_H

//
// Variant serializer into a byte array
//

#include "IVariantSerializer.h"
#include "ByteArray.h"

namespace ucxx {

/**
 * Implementation of IVariantSerializer interface.
 * Data is serialized into a byte array.
 */
class ByteArraySerializer : public IVariantSerializer
{
public:
    ByteArraySerializer();
    ByteArraySerializer(const ByteArray &ba);

    void initWith(const ByteArray &ba);

    size_t available() const;

    // IVariantSerializer interface
    void pushValue(const Variant &value);
    bool popValue(Variant &v);

    // Reset read index to zero.
    void reset();

    const ByteArray& byteArray() const { return m_byteArray; }

private:

    void pushTypeSignature(Variant::Type type);
    bool popTypeSignature(Variant::Type &type);

    void pushInvalid();
    void pushNull();
    void pushBoolean(bool value);
    void pushInteger(int value);
    void pushReal(double value);
    void pushString(const std::string &value);
    void pushList(const VariantList &value);
    void pushMap(const VariantMap &value);

    bool popBoolean(bool &value);
    bool popInteger(int &value);
    bool popReal(double &value);
    bool popString(std::string &value);
    bool popList(VariantList &value);
    bool popMap(VariantMap &value);

    ByteArray m_byteArray;	///< Internal byte array serialization buffer.
    unsigned m_index;   	///< Read index.
};

} // namespace ucxx

#endif // UCXX_BYTEARRAYSERIALIZER_H
