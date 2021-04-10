#pragma once
#include <span>
#include <vector>

class IDataReader
{
  public:
    /* Virtual destructor ensures correct destructor would always be called on polymorphic delete */
    virtual ~IDataReader() = default;
    /* Returning a vector would mean an allocation would happen on every read. That would not be the C++ way. */
    virtual size_t Read(std::vector<std::byte>& data, size_t count) = 0;
    /* Seeking not supported in network traffic, will not be part of this base interface */
};

class IDataWriter
{
  public:
    virtual ~IDataWriter() = default;
    virtual void Write(const std::vector<std::byte>& data) = 0;
};

/* Files and memory buffers support random access, network does not */
class ISeekable
{
  public:
    virtual ~ISeekable() = default;

    virtual void Seek(size_t new_position) = 0;
    [[nodiscard]] virtual size_t GetSize() = 0;
};
