#include "ByteArraySerializer.h"

namespace ucxx {

const char cTypeSignature[Variant::MaxTypes] = {
    'X',    // Invalid
	'N',	// Null
    'B',    // Boolean
    'I',    // Integer
    'R',    // Real
    'S',    // String
    'L',    // List
    'M'     // Map
};

std::map<char, Variant::Type> createSignatureToTypeMap()
{
	std::map<char, Variant::Type> map;
	map['X'] = Variant::Type_Invalid;
	map['N'] = Variant::Type_Null;
	map['B'] = Variant::Type_Boolean;
	map['I'] = Variant::Type_Integer;
	map['R'] = Variant::Type_Real;
	map['S'] = Variant::Type_String;
	map['L'] = Variant::Type_List;
	map['M'] = Variant::Type_Map;
	return map;
}
const std::map<char, Variant::Type> cSignatureToTypeMap = createSignatureToTypeMap();

template <typename T>
void pushRawValue(T value, ByteArray &byteArray)
{
	char buffer[sizeof(T)];
	T *pValue = reinterpret_cast<T*>(buffer);
	*pValue = value;
	byteArray.append(buffer, sizeof(T));
}

template <typename T>
bool popRawValue(const ByteArray &byteArray, unsigned &index, T &value)
{
	if (byteArray.size() - index < sizeof(T)) {
		return false;
	}
	const char *pBuffer = byteArray.constData() + index;
	value = *reinterpret_cast<const T*>(pBuffer);
	index += sizeof(T);
	return true;
}

ByteArraySerializer::ByteArraySerializer()
    : m_byteArray()
{
    reset();
}

ByteArraySerializer::ByteArraySerializer(const ByteArray &ba)
    : m_byteArray(ba)
{
    reset();
}

void ByteArraySerializer::initWith(const ByteArray &ba)
{
	m_byteArray = ba;
	reset();
}

size_t ByteArraySerializer::available() const
{
    return m_byteArray.size() - m_index;
}

void ByteArraySerializer::pushValue(const Variant &value)
{
	switch (value.type()) {
	case Variant::Type_Null:
		pushNull();
		break;
	case Variant::Type_Boolean:
		pushBoolean(value.toBoolean());
		break;
	case Variant::Type_Integer:
		pushInteger(value.toInteger());
		break;
	case Variant::Type_Real:
		pushReal(value.toReal());
		break;
	case Variant::Type_String:
		pushString(value.string());
		break;
	case Variant::Type_List:
		pushList(value.list());
		break;
	case Variant::Type_Map:
		pushMap(value.map());
		break;
	default:
		// Serializing invalid value
		pushInvalid();
		break;
	}
}

bool ByteArraySerializer::popValue(Variant &value)
{
    if (available() <= 0) {
        return false;
    }

    Variant::Type type = Variant::Type_Invalid;

    if (!popTypeSignature(type)) {
    	return false;
    }

    switch (type) {
    case Variant::Type_Invalid:
    	value = Variant(Variant::Type_Invalid);
    	break;
    case Variant::Type_Null:
    	value = Variant(Variant::Type_Null);
    	break;
    case Variant::Type_Boolean: {
    	bool v;
    	if (!popBoolean(v)) {
    		return false;
    	}
    	value = v;
    	break;
    }
    case Variant::Type_Integer: {
    	int v;
    	if (!popInteger(v)) {
    		return false;
    	}
    	value = v;
    	break;
    }
    case Variant::Type_Real: {
    	double v;
    	if (!popReal(v)) {
    		return false;
    	}
    	value = v;
    	break;
    }
    case Variant::Type_String: {
    	std::string v;
    	if (!popString(v)) {
    		return false;
    	}
    	value = v;
    	break;
    }
    case Variant::Type_List: {
    	VariantList v;
    	if (!popList(v)) {
    		return false;
    	}
    	value = v;
    	break;
    }
    case Variant::Type_Map: {
    	VariantMap v;
    	if (!popMap(v)) {
    		return false;
    	}
    	value = v;
    	break;
    }
    default:
    	return false;
    }

    return true;
}

void ByteArraySerializer::reset()
{
    m_index = 0;
}

void ByteArraySerializer::pushTypeSignature(Variant::Type type)
{
	m_byteArray.append(cTypeSignature[type]);
}

bool ByteArraySerializer::popTypeSignature(Variant::Type &type)
{
	if (available() < 1) {
		return false;
	}

	char c = m_byteArray.constData()[m_index];
	std::map<char, Variant::Type>::const_iterator it = cSignatureToTypeMap.find(c);
	if (it == cSignatureToTypeMap.end()) {
		// Invalid signature
		return false;
	}

	type = it->second;
	++m_index;
	return true;
}

void ByteArraySerializer::pushInvalid()
{
	pushTypeSignature(Variant::Type_Invalid);
}

void ByteArraySerializer::pushNull()
{
	pushTypeSignature(Variant::Type_Null);
}

void ByteArraySerializer::pushBoolean(bool value)
{
	pushTypeSignature(Variant::Type_Boolean);
	pushRawValue<bool>(value, m_byteArray);
}

void ByteArraySerializer::pushInteger(int value)
{
	pushTypeSignature(Variant::Type_Integer);
	pushRawValue<int>(value, m_byteArray);
}

void ByteArraySerializer::pushReal(double value)
{
	pushTypeSignature(Variant::Type_Real);
	pushRawValue<double>(value, m_byteArray);
}

void ByteArraySerializer::pushString(const std::string &value)
{
	pushTypeSignature(Variant::Type_String);
	unsigned length = value.length();
	pushRawValue<unsigned>(length, m_byteArray);
	m_byteArray.append(value.c_str(), length);
}

void ByteArraySerializer::pushList(const VariantList &value)
{
	pushTypeSignature(Variant::Type_List);
	unsigned size = value.size();
	pushRawValue<unsigned>(size, m_byteArray);
	for (VariantList::const_iterator it = value.begin(); it != value.end(); ++it) {
		pushValue(*it);
	}
}

void ByteArraySerializer::pushMap(const VariantMap &value)
{
	pushTypeSignature(Variant::Type_Map);
	unsigned size = value.size();
	pushRawValue<unsigned>(size, m_byteArray);
	for (VariantMap::const_iterator it = value.begin(); it != value.end(); ++it) {
		pushValue(Variant(it->first));
		pushValue(it->second);
	}
}

bool ByteArraySerializer::popBoolean(bool &value)
{
	return popRawValue<bool>(m_byteArray, m_index, value);
}

bool ByteArraySerializer::popInteger(int &value)
{
	return popRawValue<int>(m_byteArray, m_index, value);
}

bool ByteArraySerializer::popReal(double &value)
{
	return popRawValue<double>(m_byteArray, m_index, value);
}

bool ByteArraySerializer::popString(std::string &value)
{
	unsigned length = 0;
	if (!popRawValue<unsigned>(m_byteArray, m_index, length)) {
		return false;
	}

	if (available() < length) {
		return false;
	}

	value = std::string(m_byteArray.constData() + m_index, length);
	m_index += length;
	return true;
}

bool ByteArraySerializer::popList(VariantList &value)
{
	unsigned length = 0;
	if (!popRawValue<unsigned>(m_byteArray, m_index, length)) {
		return false;
	}

	VariantList list;
	for (unsigned i = 0; i < length; i++) {
		Variant v;
		if (!popValue(v)) {
			return false;
		}
		list.push_back(v);
	}
	value = list;
	return true;
}

bool ByteArraySerializer::popMap(VariantMap &value)
{
	unsigned length = 0;
	if (!popRawValue<unsigned>(m_byteArray, m_index, length)) {
		return false;
	}

	VariantMap map;
	for (unsigned i = 0; i < length; i++) {
		Variant key;
		if (!popValue(key)) {
			return false;
		}
		if (key.type() != Variant::Type_String) {
			return false;
		}
		Variant v;
		if (!popValue(v)) {
			return false;
		}
		map[key.string()] = v;
	}

	value = map;
	return true;
}

} // namespace ucxx
