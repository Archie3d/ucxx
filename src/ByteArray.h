#ifndef UCXX_BYTEARRAY_H
#define UCXX_BYTEARRAY_H

//
// An array of bytes
//

#include <string>
#include <vector>

namespace ucxx {

/**
 * @brief An array of bytes.
 * This class is basically a wrapper around std::vector<char> providing
 * some convenient methods to access the bytes.
 */
class ByteArray
{
public:

    /**
     * @brief Construct an empty byte array.
     */
    ByteArray();

    /**
     * @brief Construct byte array from a string.
     * @param str Input string.
     */
    ByteArray(const std::string &str);

    /**
     * @brief Construct byte array from a raw buffer of characters.
     * Input buffer will be copied into the byte array constructed.
     * @param pBuffer Pointer to the input buffer of characters.
     * @param size Size of the input buffer.
     */
    ByteArray(const char *pBuffer, size_t size);

    /**
     * @brief Construct byte array from a vector of characters.
     * @param v Input vector of characters.
     */
    ByteArray(const std::vector<char> &v);

    /**
     * @brief Copy constructor.
     * @param ba Byte array to be copied.
     */
    ByteArray(const ByteArray &ba);

    /**
     * @brief Assignment operator.
     * @param ba Byte array to be assigned.
     */
    ByteArray& operator =(const ByteArray &ba);

    /**
     * @brief Returns current size of the byte array.
     * @return Number of bytes in this byte array.
     */
    size_t size() const;

    /**
     * @brief Tells whether this byte array is empty.
     * An empty byte array contains no bytes.
     * @return true if this byte array is empty.
     */
    bool isEmpty() const { return size() == 0; }

    /**
     * @brief Append a character (byte) to the tail of this byte array.
     * @param b Byte to be added.
     */
    void append(char b);

    /**
     * @brief Append another byte array to the end of this one.
     * @param ba Byte array to be appended.
     */
    void append(const ByteArray &ba);

    /**
     * @brief Append a raw buffer to the end of this byte array.
     * @param pBuffer Pointer to the buffer to be added.
     * @param size Number of bytes in the buffer.
     */
    void append(const char *pBuffer, size_t size);

    /**
     * @brief Append a string to this byte array.
     * @param str String to be added.
     */
    void append(const std::string &str);

    /**
     * @brief Remove all data from this byte array.
     * @post Byte array resulting size is set to zero.
     */
    void clear();

    /**
     * @brief Access a byte at given position.
     * @note This may throw runtime exception if index is outside of the bounds.
     * @param i Byte's position within this byte array.
     * @return Byte reference.
     */
    char& operator [](int i);

    /**
     * @brief Access a byte at given position.
     * @note This may throw runtime exception if index is outside of the bounds.
     * @param i Byte's position within this byte array.
     * @return Byte value.
     */
    char operator [](int i) const;

    /**
     * @brief Extract a slice from this byte array.
     * This will construct a new byte array, which contains a fraction of the original one.
     * Original (this) byte array is not modified.
     * @note This may throw a runtime exception if specified slice range is out of bounds.
     * @param begin Index of the first element of the slice.
     * @param size Number of bytes in the slice.
     * @return Byte array's slice.
     */
    ByteArray slice(int begin, size_t size) const;

    /**
     * @brief Returns raw pointer to internal data.
     * @return Internal buffer pointer.
     */
    char* data();

    /**
     * @brief Returns raw pointer to internal data.
     * @return Internal buffer pointer.
     */
    const char* constData() const;

private:

    /// Internal byte array representation.
    std::vector<char> m_data;
};

} // namespace ucxx

#endif // UCXX_BYTEARRAY_H
