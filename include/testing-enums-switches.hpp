#pragma once

#include <array>
#include <tuple>
#include <string_view>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <optional>
#include <stdexcept>

//https://www.youtube.com/watch?v=INn3xa4pMfg
template <typename Key, typename Value, std::size_t Size>
struct static_map {
    std::array<std::pair<Key,Value>, Size> data;
    [[nodiscard]] constexpr std::optional<Value> at(const Key &key) const noexcept {
        const auto it = std::find_if(begin(data),end(data),
                [&key](const auto &val) {return val.first == key;} );
        if(it != end(data))
            return it->second;
        return std::nullopt;
    };
    [[nodiscard]] constexpr Value operator[](const Key &key) const {
        const auto it = std::find_if(begin(data),end(data),
                [&key](const auto &val) {return val.first == key;} );
        if(it != end(data))
            return it->second;
        else
            throw std::range_error("Key not found");
    };

    [[nodiscard]] constexpr static_map<Value,Key,Size> switch_key_value() const noexcept {
        constexpr std::array<std::pair<Value,Key>, Size> ret;
        for(std::size_t i = 0; i < data.size(); ++i) {
            ret[i] = { data[i].second, data[i].first};
        }
        return static_map<Value,Key,Size>{ret};
    }
};


enum class testenum { value1, value2, value3, value4};


struct enum_traits {
   // static_assert();
};

using testenum_pair = std::pair<const testenum, const std::string_view>;
namespace {
    using namespace std::literals::string_view_literals;
}
constexpr const std::array<testenum_pair,4> testenum_map {{ {testenum::value1,"value1"sv},
                                                           {testenum::value2,"value2"sv},
                                                           {testenum::value3,"value3"sv},
                                                           {testenum::value4,"value4"sv} }};
constexpr const static_map<std::string_view,testenum,4> testenum_map2 { { { {"value1"sv, testenum::value1},
                                                           {"value2"sv,testenum::value2},
                                                           {"value3"sv,testenum::value3},
                                                           {"value4"sv,testenum::value4} } } };

template<typename T, std::size_t MapSize>
[[nodiscard]] constexpr std::enable_if_t<std::is_enum_v<T>,std::optional<T>> from_string(const std::string_view sv, const static_map<std::string_view,T,MapSize>& sv_map) noexcept
{
    return sv_map.at(sv);
}

static_assert(from_string("value2"sv, testenum_map2).value() == testenum::value2);

struct enum_map
{

};

constexpr void check_enum()
{

}

template <typename T, T val, const char* U>
struct enum_name 
{
    static_assert(std::is_enum_v<T>);
    using type = T;
    constexpr static type value() {
        return val;
    };
    static constexpr std::string_view name { U };
};

constexpr const char value1_str[] = "value1";
enum_name<testenum, testenum::value1, value1_str> myname;

static_assert(std::is_same_v<testenum, decltype(testenum::value1)>);

//static_assert(std::is_same_v<testenum, decltype(testenum::value5)>);

template <typename T, T val>
struct count_enum {

};


//constexpr const std::array<std::string_view,> testenum_string;

