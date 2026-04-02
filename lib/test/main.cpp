#include <unordered_map>
#include <string>
#include <chrono>
#include <iostream>

#include <alfred/hash.hpp>

int main() {
    using namespace hash::literals;

    std::unordered_map<unsigned int, std::string> map1;
    std::unordered_map<hash::HashedStr32, std::string, hash::StrHash<hash::HashedStr32>> map2;

    map1[1] = "one";
    map1[2] = "two";
    map1[3] = "three";
    map1[4] = "four";
    map1[5] = "five";
    map1[6] = "six";

    map2["one123k"_h] = "one";
    map2["two123ui"_h] = "two";
    map2["three123"_h] = "three";
    map2["four123j"_h] = "four";
    map2["five123b"_h] = "five";
    map2["six123"_h] = "six";

    std::cout << map1.load_factor() << '\n';
    std::cout << map2.load_factor() << '\n';

    unsigned long long ret {};

    {
        auto start = std::chrono::high_resolution_clock::now();

        for (int i {}; i < 100'000'000; i++) {
            ret -= map1.at(2).at(1);
            ret -= map1.at(5).at(0);
            ret -= map1.at(1).at(2);
            ret -= map1.at(4).at(1);
        }

        auto stop = std::chrono::high_resolution_clock::now();

        std::cout << "map1 " << std::chrono::duration_cast<std::chrono::seconds>(stop - start) << '\n';
    }

    {
        auto start = std::chrono::high_resolution_clock::now();

        for (int i {}; i < 100'000'000; i++) {
            ret += map2.at("two123ui"_h).at(1);
            ret += map2.at("five123b"_h).at(0);
            ret += map2.at("one123k"_h).at(2);
            ret += map2.at("four123j"_h).at(1);
        }

        auto stop = std::chrono::high_resolution_clock::now();

        std::cout << "map2 " << std::chrono::duration_cast<std::chrono::seconds>(stop - start) << '\n';
    }

    return ret;
}
