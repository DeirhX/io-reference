// Homework.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <span>

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
    /* Virtual destructor ensures correct destructor would always be called on polymorphic delete */
    virtual ~IDataWriter() = default;
    virtual void Write(const std::vector<std::byte> & data) = 0;
};

/* Files and memory buffers support random access, network does not */
class ISeekable
{
public:
    virtual ~ISeekable() = default;

    virtual void Seek(size_t new_position) = 0;
    [[nodiscard]] virtual size_t GetSize() = 0;
};


/* Will be virtually inherited so it can serve as a base even for combiner reader-writer */
class MemoryStorage : public ISeekable
{
  protected:
    std::span<std::byte> raw_data;
    size_t position = 0;
    /* No direct instantiation. Don't confuse the user, no useful functions can be performed. */
    MemoryStorage() = default;
  public:
    /* We do not own the backing store - the point of a dedicated reader is to be able to read from already allocated buffers */
    MemoryStorage(std::span<std::byte> data) : raw_data(data) {}
    MemoryStorage(std::vector<std::byte>& data) : raw_data(data) {}
    MemoryStorage(std::byte* data, size_t data_size) : raw_data(data, data_size) { assert(data); }

    virtual void Seek(size_t new_position) { this->position = new_position; }
    [[nodiscard]] virtual size_t GetSize() { return this->position; }
};

class MemoryReader : public virtual MemoryStorage, public IDataReader
{
  public:
    /* Inherit all constructors */
    using MemoryStorage::MemoryStorage;

    size_t Read(std::vector<std::byte>& data, size_t count) override
    {
        /* Read only as much there is no read. */
        count = std::min(count, raw_data.size() - (position + count));

        data.resize(count);
        std::copy_n(raw_data.begin() + position, count, data.begin());
        position += count;

        return count;
    }
};

class MemoryWriter : public virtual MemoryStorage, public IDataWriter
{
public:
    /* Inherit all constructors */
    using MemoryStorage::MemoryStorage;

    void Write(const std::vector<std::byte>& data) override
    {
        if (data.size() > raw_data.size() - position)
            throw std::exception("Attempted to write past the end of non-resizable buffer");
        std::copy_n(data.begin(), data.size(), raw_data.begin() + position);
        position += data.size();
    }
};



class FileStorage
{
    std::basic_fstream<std::byte> file_stream;

  protected:
    enum OpenFlags
    {
        Read = 0x1,
        Write = 0x2,
    };

  public:
    FileStorage(const char* file_name, OpenFlags operation)
        : file_stream(file_name, ((operation & OpenFlags::Read) ? std::ios_base::in : 0) | ((operation & OpenFlags::Write) ? std::ios_base::out : 0))
    {
        assert(operation != 0);
        if (!file_stream.is_open() || !file_stream.good())
            throw std::exception("Failed to open file");
    }
};

int main()
{
    auto mem_data = std::vector<std::byte>{std::byte('a')};
    auto reader = MemoryReader(mem_data);
    std::cout << "Hello World!\n";
}


