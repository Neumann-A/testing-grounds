#pragma once
// #if _MSVC_LANG <= 201703L
//     #define _MSVC_LANG 201803L
//     #pragma message("test")
// #else
//     #pragma message("othertest")
// #endif

#include <array>
#include <tuple>
#include <string_view>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <optional>
#include <stdexcept>
#include <tuple>

#ifdef _HAS_CXX20
    #pragma message("HASCXX20")
#endif
//https://www.youtube.com/watch?v=INn3xa4pMfg
template <typename Key, typename Value, std::size_t Size>
struct static_map {
    using type = static_map<Key,Value,Size>;
    using size_type = std::size_t;
    using value_type = std::pair<Key,Value>;
    std::array<value_type, Size> data;
    [[nodiscard]] constexpr std::optional<Value> at(const Key &key) const noexcept {
        const auto it = std::find_if(std::begin(data),std::end(data),
                [&key](const auto &val) {return val.first == key;} );
        if(it != std::end(data))
            return it->second;
        return std::nullopt;
    };

    [[nodiscard]] constexpr value_type& operator[](size_type index) {
        return data[index];
    };
    [[nodiscard]] constexpr const value_type& operator[](size_type index) const {
        return this->operator[](index);
    };

    [[nodiscard]] constexpr Value operator[](const Key &key) const {
        const auto it = std::find_if(begin(data),end(data),
                [&key](const auto &val) {return val.first == key;} );
        if(it != end(data))
            return it->second;
        else
            throw std::range_error("Key not found");
    };
    [[nodiscard]] constexpr Value operator[](const Value &val) const {
        const auto it = std::find_if(begin(data),end(data),
                [&val](const auto &key) {return key.second == val;} );
        if(it != end(data))
            return it->first;
        else
            throw std::range_error("Value not found");
    };

    [[nodiscard]] constexpr auto begin() noexcept {
        return begin(data);
    }
    [[nodiscard]] constexpr auto begin() const noexcept {
        return begin(data);
    }
    [[nodiscard]] constexpr auto end() noexcept {
        return end(data);
    }
    [[nodiscard]] constexpr auto end() const noexcept {
        return end(data);
    }
    [[nodiscard]] constexpr static_map<Value,Key,Size> switch_key_value() const noexcept {
        std::array<std::pair<Value,Key>, Size> ret;
        for(std::size_t i = 0; i < data.size(); ++i) {
            ret[i] = std::make_pair(Value{data[i].second}, Key{data[i].first});
        }
        return static_map<Value,Key,Size>{ret};
    }
};

template<typename EnumType, EnumType Val>
struct EnumValue
{
    using type = EnumType;
    static constexpr EnumType value = Val;
};

template<typename EnumType, EnumType Val1 , EnumType... Vals>
struct EnumNode {
        using type = EnumType;
        static constexpr EnumType value = Val1;
        using next = EnumNode<EnumType, Vals...>;
        //auto next(EnumType... Vals) { return EnumNode<EnumType,Vals...>();};
};



template<typename EnumType, EnumType... Vals>
struct enum_type_value_list : std::integer_sequence<EnumType, Vals...> {
    static_assert(std::is_enum_v<decltype(Val1)>);

    template <std::size_t N>
    using element_type = typename std::tuple_element<N, std::tuple<Vals...>>::type;

    static constexpr EnumType get(std::underlying_type_t<EnumType> i)
    {
        return 
    }
};


enum class testenum { value1, value2, value3, value4};

enum_type_list<testenum, testenum::value1,testenum::value2, testenum::value3, testenum::value4> test_types;
static_assert(EnumValue<testenum,testenum::value1>::value==testenum::value1);

struct enum_traits {
   // static_assert();
};

using testenum_pair = std::pair<const testenum, const std::string_view>;
namespace {
    using namespace std::literals::string_view_literals;
}
constexpr const static_map<testenum,std::string_view,4>testenum_map {{{ {testenum::value1,"value1"sv},
                                                           {testenum::value2,"value2"sv},
                                                           {testenum::value3,"value3"sv},
                                                           {testenum::value4,"value4"sv} }}};
constexpr const static_map<std::string_view,testenum,4> testenum_mapswitched {testenum_map.switch_key_value()};
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

