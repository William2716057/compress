#include <array>
#include <bitset>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
 
void printBinaryRow(const unsigned char* buf, std::size_t n, std::size_t width) {
    for (std::size_t i = 0; i < width; ++i) {
        if (i < n)
            std::cout << std::bitset<8>(buf[i]) << ' ';
        else
            std::cout << "         ";   // 9 spaces keeps the ASCII column aligned
    }
}
 
int main() {
    std::cout << "Enter file name: ";
    std::string filename;
    std::getline(std::cin, filename);
 
    std::cout << "Format (h = hex, b = binary): ";
    char mode;
    std::cin >> mode;
    bool binary = (mode == 'b' || mode == 'B');
 
    constexpr std::size_t kWidth = 16;
    const std::size_t width = binary ? 6 : 16;
    std::array<unsigned char, kWidth> buf{};   // sized for the larger case
 
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        std::cerr << "Error: cannot open " << filename << "\n";
        return 1;
    }
 
    std::uint64_t offset = 0;
 
    while (in) {
        in.read(reinterpret_cast<char*>(buf.data()), width);
        std::size_t n = static_cast<std::size_t>(in.gcount());
        if (n == 0) break;
 
        // Offset
        std::cout << std::hex << std::setfill('0') << std::setw(8) << offset << "  ";
 
        if (binary) {
            printBinaryRow(buf.data(), n, width);
        } else {
            // Hex column
            for (std::size_t i = 0; i < width; ++i) {
                if (i < n)
                    std::cout << std::setw(2) << static_cast<int>(buf[i]) << ' ';
                else
                    std::cout << "   ";
                if (i == 7) std::cout << ' ';  // extra space
            }
        }
 
        // ASCII
        std::cout << " |";
        for (std::size_t i = 0; i < n; ++i)
            std::cout << (std::isprint(buf[i]) ? static_cast<char>(buf[i]) : '.');
        std::cout << "|\n";
 
        offset += n;
    }
 
    std::cout << std::hex << std::setfill('0') << std::setw(8) << offset << "\n";

    std::cout << "Do you want to compress file? (Y) (N): ";
    char answer;
    std::cin >> answer;
    bool compress = (answer == 'y' || answer == 'Y');
    return 0;
}
