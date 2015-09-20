#include "ByteArray.h"

namespace ucxx {

ByteArray::ByteArray()
    : m_data()
{
}

ByteArray::ByteArray(const std::string &str)
    : m_data()
{
    size_t l = str.length();
    const char *pStr = str.c_str();
    m_data.assign(pStr, pStr + l);
}

ByteArray::ByteArray(const char *pBuffer, size_t size)
    : m_data()
{
    m_data.assign(pBuffer, pBuffer + size);
}

ByteArray::ByteArray(const std::vector<char> &v)
    : m_data(v)
{
}

ByteArray::ByteArray(const ByteArray &ba)
    : m_data(ba.m_data)
{
}

ByteArray& ByteArray::operator =(const ByteArray &ba)
{
    if (this != &ba) {
        m_data = ba.m_data;
    }
    return *this;
}

size_t ByteArray::size() const
{
    return m_data.size();
}

void ByteArray::append(char b)
{
    m_data.push_back(b);
}

void ByteArray::append(const ByteArray &ba)
{
    m_data.insert(m_data.end(), ba.m_data.begin(), ba.m_data.end());
}

void ByteArray::append(const char *pBuffer, size_t size)
{
	std::copy(&pBuffer[0], &pBuffer[size], std::back_inserter(m_data));
}

void ByteArray::append(const std::string &str)
{
    append(str.c_str(), str.length());
}

void ByteArray::clear()
{
    m_data.clear();
}

char& ByteArray::operator [](int i)
{
    return m_data[i];
}

char ByteArray::operator [](int i) const
{
    return m_data[i];
}

ByteArray ByteArray::slice(int begin, size_t size) const
{
    std::vector<char>::const_iterator first = m_data.begin() + begin;
    std::vector<char>::const_iterator last = first + size;
    return ByteArray(std::vector<char>(first, last));
}

char* ByteArray::data()
{
    return m_data.data();
}

const char* ByteArray::constData() const
{
    return m_data.data();
}

} // namespace ucxx
