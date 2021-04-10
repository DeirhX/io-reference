#include "MemoryStorage.h"
#include <iostream>

int main()
{
    auto mem_data = std::vector<std::byte>{std::byte('a')};
    auto reader = MemoryReader(mem_data);
    auto readwrite = MemoryReaderWriter(mem_data);
    std::cout << "Hello World!\n";
}


