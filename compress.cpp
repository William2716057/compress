#include <array>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

int main() {
    std::cout << "Enter file name: ";
    std::string filename;
    std::getline(std::cin, filename);

    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        std::cerr << "Error: cannot open " << filename << "\n";
        return 1;
    }

    constexpr std::size_t kWidth = 16;
    std::array<unsigned char, kWidth> buf{};
    std::uint64_t offset = 0;

    while (in) {
        in.read(reinterpret_cast<char*>(buf.data()), kWidth);
        std::size_t n = static_cast<std::size_t>(in.gcount());
        if (n == 0) break;

        // Offset
        std::cout << std::hex << std::setfill('0') << std::setw(8) << offset << "  ";

        // Hex column
        for (std::size_t i = 0; i < kWidth; ++i) {
            if (i < n)
                std::cout << std::setw(2) << static_cast<int>(buf[i]) << ' ';
            else
                std::cout << "   ";
            if (i == 7) std::cout << ' ';  // extra space
        }

        // ASCII
        std::cout << " |";
        for (std::size_t i = 0; i < n; ++i)
            std::cout << (std::isprint(buf[i]) ? static_cast<char>(buf[i]) : '.');
        std::cout << "|\n";

        offset += n;
    }

    std::cout << std::hex << std::setfill('0') << std::setw(8) << offset << "\n";
    return 0;
}
