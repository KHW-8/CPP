#include <iostream>
#include <bit>
#include <cstdint>
#include <array>
#include <vector>

// Separate 1 float into 4 uint8_t bytes
std::array<uint8_t, 4> floatToBytes(float input) {
    return std::bit_cast<std::array<uint8_t, 4>>(input);
}

// Reassemble array of bytes to float
float bytesToFloat(const std::array<uint8_t, 4>& bytes) {
    return std::bit_cast<float>(bytes);
}

int main() {
    float original = 3.1415926f;

    // Split
    auto arr = floatToBytes(original);

    std::cout << "Bytes: ";
    for (int i = 0; i < 4; ++i) {
        std::cout << "0x" << std::hex << static_cast<int>(arr[i]) << " ";
    }
    std::cout << "\n";

    // Reassemble
    float reassembled = bytesToFloat(arr);
    std::cout << std::dec << "Reassembled: " << reassembled << "\n";

    return 0;
}