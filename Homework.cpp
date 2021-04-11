#include "FileStorage.h"
#include "MemoryStorage.h"
#include <iostream>
#include <array>

int main()
{
    std::array<std::byte, 15> memory_buffer = {};
    std::memcpy(&memory_buffer, "binary\0data\0\0\0\0", 15);

    auto print_to_console = [](auto& container) {
        for (auto elem : container)
            std::cout << static_cast<unsigned char>(elem);
        std::cout << std::endl;
    };

    {
        /* Test MemoryReader */
        auto reader = MemoryReader(memory_buffer);
        auto read_buf = std::vector<std::byte>{};
        reader.Read(read_buf, reader.GetSize());
        print_to_console(read_buf);

        /* Test MemoryWriter + MemoryReader */
        auto writer = MemoryWriter(memory_buffer);
        writer.Write(std::vector<std::byte>{6, std::byte{'='}});
        reader.Seek(0);
        reader.Read(read_buf, reader.GetSize());
        print_to_console(read_buf);

        /* Test MemoryReaderWriter*/
        auto readwrite = MemoryReaderWriter(memory_buffer);
        readwrite.Seek(12);
        readwrite.Write(std::vector<std::byte>{3, std::byte{'='}});
        readwrite.Seek(12);
        readwrite.Read(std::span{read_buf.begin() + 12, read_buf.end()});
        print_to_console(read_buf);
    }

    {
        /* Test FileWriter + FileReader */
        {
            auto writer = FileWriter("test.bin");
            writer.Write(memory_buffer);
            /* Flushed + closed */
        }
        auto reader = FileReader("test.bin");
        auto read_buf = std::vector<std::byte>{};
        /* Intentionally try to read more */
        auto read_count = reader.Read(read_buf, memory_buffer.size() + 10);
        assert(read_count == memory_buffer.size());
        print_to_console(read_buf);

        /* Test FileReaderWriter*/
        auto readwrite = FileReaderWriter("test.bin");
        readwrite.Write(std::vector<std::byte>{6, std::byte{'*'}});
        readwrite.Seek(0);
        readwrite.Read(read_buf, readwrite.GetSize());
        print_to_console(read_buf);
    }
}


