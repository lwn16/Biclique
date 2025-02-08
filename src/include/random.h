#include <random>
#include <vector>
#include <algorithm>

extern std::mt19937_64 rnd_gen;

template<class T>
void Shuffle(std::vector<T> &v) {
    std::shuffle(v.begin(), v.end(), rnd_gen);
}

template<class T>
auto RandomInt(T l, T r) -> T {
    std::uniform_int_distribution<T> uni_distri(l, r - 1);
    return uni_distri(rnd_gen);
}


auto Random(int64_t total) -> int64_t;
auto Random(double total) -> double;
