#pragma once
#include <span>
#include <vector>

class IDataReader
{
  public:
    /* Virtual destructor ensures correct destructor would always be called on polymorphic delete */
    virtual ~IDataReader() = default;
    /* Read as much as {count} bytes from the source if available.
     * Returns number of bytes actually read. Throws if there is non-recoverable error in the source medium.
     * Note: Returning a vector would mean an allocation would happen on every read. That would not be the C++ way. */
    virtual size_t Read(std::vector<std::byte>& out_data, size_t count) = 0;
    /* The following is just a bonus efficiency signature */
    /* Reading into a span fragment guarantees no allocation and a part of a contiguous container can be passed in easily  */
    virtual size_t Read(std::span<std::byte> out_data) = 0;
    /* Seeking not supported in network traffic, will not be part of this base interface */
};

class IDataWriter
{
  public:
    virtual ~IDataWriter() = default;
    /* Write entire span/vector to destination buffer. Throws if cannot be completed entirely. */
    virtual void Write(std::span<const std::byte> data) = 0;
    /* Note: This signature is here readability & IntelliSense only, this conversion is done automatically anyway */
    void Write(const std::vector<std::byte>& data) { Write(std::span{data}); }
};

/* Files and memory buffers support random access, network does not */
class ISeekable
{
  public:
    virtual ~ISeekable() = default;

    virtual void Seek(size_t new_position) = 0;
    [[nodiscard]] virtual size_t GetSize() = 0;
};
