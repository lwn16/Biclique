#include "random.h"

std::mt19937_64 rnd_gen((std::random_device())());

auto Random(int64_t total) -> int64_t {
    return RandomInt(int64_t(0), total);
}
auto Random(double total) -> double {
    return std::uniform_real_distribution<double>(0, total)(rnd_gen);
}
