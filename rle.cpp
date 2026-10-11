#include <array>
#include <bitset>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

void printBinaryRow(const unsigned char* buf, std::size_t n, std::size_t width) {
    for (std::size_t i = 0; i < width; ++i) {
        if (i < n)
            std::cout << std::bitset<8>(buf[i]) << ' ';
        else
            std::cout << "         ";   // 9 spaces keeps ASCII column aligned
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

    in.clear();      // the dump loop left the stream at EOF
    in.seekg(0);     // rewind to start of the file

    // Load the whole file so it can be encrypted and drawn
    std::vector<unsigned char> data((std::istreambuf_iterator<char>(in)),
                                    std::istreambuf_iterator<char>());

    std::cout << "Do you want to encrypt file? (Y) (N): ";
    char answer;
    std::cin >> answer;
    bool encrypt = (answer == 'y' || answer == 'Y');

    if (encrypt) {
        std::cout << "Enter passphrase (no spaces): ";
        std::string key;
        std::cin >> key;

        // XOR each byte with the passphrase, repeating it. Running the same
        // step again with the same passphrase restores the original bytes.
        for (std::size_t i = 0; i < data.size(); ++i)
            data[i] ^= static_cast<unsigned char>(key[i % key.size()]);

        const std::string outName = filename + ".enc";
        std::ofstream out(outName, std::ios::binary);
        if (!out) {
            std::cerr << "Error: cannot create " << outName << "\n";
            return 1;
        }
        out.write(reinterpret_cast<const char*>(data.data()),
                  static_cast<std::streamsize>(data.size()));
        std::cout << "Encrypted to " << outName << "\n";
    }

    // Render the file's bits (encrypted if you chose to) as a w x h picture
    int w, h;
    std::cout << "Enter width and height: ";
    std::cin >> w >> h;

    const std::size_t total = static_cast<std::size_t>(w) * h;
    std::string stars;
    for (std::size_t i = 0; i < data.size() && stars.size() < total; ++i) {
        for (int bit = 7; bit >= 0; --bit)
            stars += ((data[i] >> bit) & 1) ? '*' : ' ';
    }
    stars.resize(total, ' ');   // trim extra bits, or pad if the file is too short

    for (int row = 0; row < h; row++) {
        std::cout << "|" << stars.substr(row * w, w) << "|" << std::endl;
    }
    return 0;
}
