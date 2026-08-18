#include <print>

auto main() -> int {
    float f1 = 0.5;

    uint8_t arr[4] = {};

    std::memcpy(arr, &f1, sizeof(float));

    for (auto i = 0; i < 4; i++)
        std::print("{}\n", arr[i]);

    float f2 = (float)*(float*)arr;

    std::print("{}\n", f2);

    return 0;
}