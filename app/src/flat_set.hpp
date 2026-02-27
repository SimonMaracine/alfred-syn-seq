#pragma once

#ifdef __cpp_lib_flat_set
    #include <flat_set>

    template<typename T>
    using std_flat_set = std::flat_set<T>;

    template<typename T>
    using std_flat_multiset = std::flat_multiset<T>;
#else
    #include <set>

    template<typename T>
    using std_flat_set = std::set<T>;

    template<typename T>
    using std_flat_multiset = std::multiset<T>;
#endif
