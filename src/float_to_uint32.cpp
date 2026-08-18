#include <iostream>
#include <cstring>
#include <cstdint>

// Separate 1 float into 4 uint8_t bytes
void floatToBytes(float input, uint8_t bytes[4]) {
    std::memcpy(bytes, &input, sizeof(float));
}

// Reassemble 4 uint8_t bytes into 1 float
float bytesToFloat(const uint8_t bytes[4]) {
    float output;
    std::memcpy(&output, bytes, sizeof(float));
    return output;
}

int main() {
    float original = 3.1415926f;
    uint8_t bytes[4];

    // Split
    floatToBytes(original, bytes);

    std::cout << "Bytes: ";
    for (int i = 0; i < 4; ++i) {
        std::cout << "0x" << std::hex << static_cast<int>(bytes[i]) << " ";
    }
    std::cout << "\n";

    // Reassemble
    float reassembled = bytesToFloat(bytes);
    std::cout << std::dec << "Reassembled: " << reassembled << "\n";

    return 0;
}