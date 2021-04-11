#pragma once
#include "Interface.h"
#include <cassert>
#include <span>
#include <vector>

/* Will be virtually inherited so it can serve as a base even for combiner reader-writer */
class MemoryStorage : public ISeekable
{
  protected:
    std::span<std::byte> buffer;
    size_t position = 0;
    /* No direct instantiation. Don't confuse the user, no useful functions can be performed. */
    MemoryStorage() = default;

  public:
    /* We do not own the backing store - the point of a dedicated reader is to be able to read from already allocated buffers */
    MemoryStorage(std::span<std::byte> source_data) : buffer(source_data) {}
    MemoryStorage(std::vector<std::byte>& source_data) : buffer(source_data) {}
    MemoryStorage(std::byte* source_data, size_t data_size) : buffer(source_data, data_size) { assert(source_data); }

    virtual void Seek(size_t new_position) { position = new_position; }
    [[nodiscard]] virtual size_t GetSize() { return buffer.size(); }
};

class MemoryReader : public virtual MemoryStorage, public IDataReader
{
  public:
    /* Inherit all constructors */
    using MemoryStorage::MemoryStorage;

    size_t Read(std::vector<std::byte>& out_data, size_t count) override
    {
        /* Read only as much there is to read. I choose not to throw an exception here and stream readers are expected to return less bytes than requested
         * if no more are ready at this time. But throwing can be easily justified just as well. */
        count = std::min(count, buffer.size() - position);
        /* resize() does *not* release extra memory, there will be no reallocation once buffer grows large enough (unless shrink_to_fit is used) */
        out_data.resize(count);
        std::copy_n(buffer.begin() + position, count, out_data.begin());
        position += count;

        return count;
    }
    size_t Read(std::span<std::byte> out_data) override
    {
        const auto count = std::min(out_data.size(), buffer.size() - position);
        std::copy_n(buffer.begin() + position, count, out_data.begin());
        position += count;
        return count;
    }
};

class MemoryWriter : public virtual MemoryStorage, public IDataWriter
{
  public:
    /* Inherit all constructors */
    using MemoryStorage::MemoryStorage;

    void Write(std::span<const std::byte> data) override
    {
        /* Writing past end-of-buffer should throw - in contrast to writing to file which can be extended as long there is enough space */
        if (data.size() > buffer.size() - position)
            throw std::exception("Attempted to write past the end of non-resizable buffer");
        std::copy_n(data.begin(), data.size(), buffer.begin() + position);
        position += data.size();
    }
};

class MemoryReaderWriter : public MemoryReader, public MemoryWriter
{
public:
    /* Could use MemoryReader as well, both lead to same virtual class */
    using MemoryWriter::MemoryWriter;
};