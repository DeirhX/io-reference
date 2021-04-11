#pragma once
#include "Interface.h"
#include <cassert>
#include <exception>
#include <fstream>
#include <filesystem>

class FileStorage : public ISeekable
{
  protected:
    enum OpenFlags
    {
        Read = 0x1,
        Write = 0x2,
    };

    std::basic_fstream<std::byte> file_stream;
    size_t file_size;
    OpenFlags mode;
  public:
    FileStorage(const char* file_name, OpenFlags operation)
        : file_stream(file_name, ((operation & OpenFlags::Read) ? std::ios_base::in : 0) |
                                      ((operation & OpenFlags::Write) ? std::ios_base::out : 0) |
                                      std::ios_base::binary),
          mode(operation)
    {
        assert(operation != 0);
        if (!file_stream.is_open() || !file_stream.good())
            throw std::exception("Failed to open file");
        file_size = std::filesystem::file_size(file_name);
    }

    virtual void Seek(size_t new_position)
    {
        if (mode & OpenFlags::Read)  file_stream.seekg(new_position);
        if (mode & OpenFlags::Write) file_stream.seekp(new_position);
    }
    [[nodiscard]] virtual size_t GetSize() { return file_size; }
};

class FileReader : public virtual FileStorage, public IDataReader
{
  public:
    FileReader(const char* file_name) : FileStorage(file_name, OpenFlags::Read) {}

    size_t Read(std::vector<std::byte>& out_data, size_t count) override
    {
        /* resize does *not* release extra File, there will be no reallocation once buffer grows large enough (unless shrink_to_fit is used)*/
        out_data.resize(count);
        file_stream.read(&out_data.front(), count);
        out_data.resize(file_stream.gcount()); /* bytes actually read */
        return count;
    }
    size_t Read(std::span<std::byte> out_data) override
    {
        file_stream.read(&out_data.front(), out_data.size());
        return file_stream.gcount();
    }
};

class FileWriter : public virtual FileStorage, public IDataWriter
{
  public:
    FileWriter(const char* file_name) : FileStorage(file_name, OpenFlags::Write) {}

    void Write(std::span<const std::byte> data) override
    {
        file_stream.write(&data.front(), data.size());
        if (!file_stream.good())
            throw std::exception("Failed to write to file");
    }
};

class FileReaderWriter : public FileReader, public FileWriter
{
  public:
    /* Note this will initialize the virtual base once and correctly, calling the other constructors won't reinitialize it */
    FileReaderWriter(const char* file_name) : FileStorage(file_name, OpenFlags(OpenFlags::Read | OpenFlags::Write)),
                                              FileReader(file_name),
                                              FileWriter(file_name) {}
};