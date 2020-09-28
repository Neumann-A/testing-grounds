#pragma once

#include <algorithm>
#include <array>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include <magic_enum.hpp>

#ifdef _HAS_CXX20
#pragma message("HASCXX20")
#endif
//https://www.youtube.com/watch?v=INn3xa4pMfg
template <typename Key, typename Value, std::size_t Size>
struct static_map : std::array<std::pair<Key, Value>, Size>
{
    using type       = static_map<Key, Value, Size>;
    using array_type = std::array<std::pair<Key, Value>, Size>;
    [[nodiscard]] constexpr std::optional<Value> at(const Key &key) const noexcept
    {
        const auto it =
            std::find_if(std::begin(*this), std::end(*this), [&key](const auto &val) { return val.first == key; });
        if (it != std::end(*this))
            return it->second;
        return std::nullopt;
    };
    [[nodiscard]] constexpr Value operator[](const Key &key) const
    {
        const auto it =
            std::find_if(std::begin(*this), std::end(*this), [&key](const auto &val) { return val.first == key; });
        if (it != std::end(*this))
            return it->second;
        else
            throw std::range_error("Key not found");
    };
    [[nodiscard]] constexpr Value operator[](const Value &val) const
    {
        const auto it =
            std::find_if(std::begin(*this), std::end(*this), [&val](const auto &key) { return key.second == val; });
        if (it != std::end(*this))
            return it->first;
        else
            throw std::range_error("Value not found");
    };
    [[nodiscard]] constexpr static_map<Value, Key, Size> switch_key_value() const noexcept
    {
        std::array<std::pair<Value, Key>, Size> ret;
        for (std::size_t i = 0; i < this->size(); ++i) {
            ret[i] = std::make_pair(Value{static_cast<array_type>(*this)[i].second},
                                    Key{static_cast<array_type>(*this)[i].first});
        }
        return static_map<Value, Key, Size>{ret};
    }
};

template <typename EnumType, EnumType... EnumVals>
struct enum_tuple
{
    using enum_type = EnumType;
    static_assert(std::is_enum_v<EnumType>);
    static constexpr auto count = sizeof...(EnumVals);

    template <EnumType NewValue>
    using append_enum_value = enum_tuple<EnumType, EnumVals..., NewValue>;

    template <EnumType NewValue>
    using prepend_enum_value = enum_tuple<EnumType, NewValue, EnumVals...>;
};

template <std::size_t Index, typename EnumTuple>
struct enum_tuple_element
{
    using type                        = EnumTuple;
    static constexpr const auto index = Index;
};

template <std::size_t Index, typename EnumType, EnumType Head, EnumType... Tail>
struct enum_tuple_element<Index, enum_tuple<EnumType, Head, Tail...>>
    : enum_tuple_element<Index - 1, enum_tuple<EnumType, Tail...>>
{
    static_assert(std::is_enum_v<EnumType>);
};

template <typename EnumType, EnumType Head, EnumType... Tail>
struct enum_tuple_element<0, enum_tuple<EnumType, Head, Tail...>>
{
    using enum_type                            = EnumType;
    static constexpr const EnumType enum_value = Head;
};

template <std::size_t Index = 0, typename EnumTuple, typename EnumTuple::enum_type Value>
constexpr decltype(auto) enum_tuple_index_impl()
{
    if constexpr (enum_tuple_element<Index, EnumTuple>::enum_value == Value)
        return Index;
    else
        return enum_tuple_index_impl<Index + 1, EnumTuple, Value>();
}

template <typename EnumTuple, typename EnumTuple::enum_type Value>
constexpr auto enum_tuple_index()
{
    return enum_tuple_index_impl<0, EnumTuple, Value>();
}

template <typename EnumType, std::size_t EnumCount, std::size_t index, typename EnumTuple>
constexpr decltype(auto) make_enum_tuple_impl()
{
    if constexpr (index < EnumCount) {
        using NewTuple = typename EnumTuple::template append_enum_value<magic_enum::enum_value<EnumType>(index)>;
        return make_enum_tuple_impl<EnumType, EnumCount, index + 1, NewTuple>();
    }
    else
        return EnumTuple{};
}

template <typename EnumType>
constexpr decltype(auto) make_enum_tuple()
{
    return make_enum_tuple_impl<EnumType, magic_enum::enum_count<EnumType>(), 1,
                                enum_tuple<EnumType, magic_enum::enum_value<EnumType>(0)>>();
}

struct enum_switch
{
    template <std::size_t switch_size, typename EnumTuple, typename DefaultCaseFunctor,
              template <typename EnumTuple::enum_type> typename EnumFunctor, typename... Args>
    static constexpr decltype(auto) switch_impl(typename EnumTuple::enum_type value, Args... args)
    {
        constexpr const auto enum_startindex = EnumTuple::count - switch_size;
        if constexpr (switch_size >= 4) {
            switch (value) {
            case enum_tuple_element<enum_startindex, EnumTuple>::enum_value:
                return EnumFunctor<enum_tuple_element<enum_startindex, EnumTuple>::enum_value>{}(
                    std::forward<Args>(args)...);
            case enum_tuple_element<enum_startindex + 1, EnumTuple>::enum_value:
                return EnumFunctor<enum_tuple_element<enum_startindex + 1, EnumTuple>::enum_value>{}(
                    std::forward<Args>(args)...);
            case enum_tuple_element<enum_startindex + 2, EnumTuple>::enum_value:
                return EnumFunctor<enum_tuple_element<enum_startindex + 2, EnumTuple>::enum_value>{}(
                    std::forward<Args>(args)...);
            case enum_tuple_element<enum_startindex + 3, EnumTuple>::enum_value:
                return EnumFunctor<enum_tuple_element<enum_startindex + 3, EnumTuple>::enum_value>{}(
                    std::forward<Args>(args)...);
            default:
                return switch_impl<switch_size - 4, EnumTuple, DefaultCaseFunctor, EnumFunctor, Args...>(
                    value, std::forward<Args>(args)...);
            }
        }
        else if constexpr (switch_size >= 2) {
            switch (value) {
            case enum_tuple_element<enum_startindex, EnumTuple>::enum_value:
                return EnumFunctor<enum_tuple_element<enum_startindex, EnumTuple>::enum_value>{}(
                    std::forward<Args>(args)...);
            case enum_tuple_element<enum_startindex + 1, EnumTuple>::enum_value:
                return EnumFunctor<enum_tuple_element<enum_startindex + 1, EnumTuple>::enum_value>{}(
                    std::forward<Args>(args)...);
            default:
                return switch_impl<switch_size - 2, EnumTuple, DefaultCaseFunctor, EnumFunctor, Args...>(
                    value, std::forward<Args>(args)...);
            }
        }
        else if constexpr (switch_size == 1) {
            switch (value) {
            case enum_tuple_element<enum_startindex, EnumTuple>::enum_value:
                return EnumFunctor<enum_tuple_element<enum_startindex, EnumTuple>::enum_value>{}(
                    std::forward<Args>(args)...);
            default:
                return switch_impl<switch_size - 1, EnumTuple, DefaultCaseFunctor, EnumFunctor, Args...>(
                    value, std::forward<Args>(args)...);
            }
        }
        else {
            static_assert(switch_size == 0);
            return DefaultCaseFunctor{}(std::forward<Args>(args)...);
        }
    };

    template <typename EnumType, typename DefaultCaseFunctor, template <EnumType> typename EnumFunctor,
              typename... Args>
    static constexpr decltype(auto) run(EnumType Value, Args &&...args)
    {
        static_assert(std::is_enum_v<EnumType>);
        using EnumTuple = decltype(make_enum_tuple<EnumType>());
        return switch_impl<EnumTuple::count, EnumTuple, DefaultCaseFunctor, EnumFunctor, Args...>(
            Value, std::forward<Args>(args)...);
    };
};

enum class testenum
{
    value1,
    value2,
    value3,
    value4,
    value5,
    value6,
    value7
};
using test_enum_tuple = enum_tuple<testenum, testenum::value1, testenum::value2, testenum::value3, testenum::value4,
                                   testenum::value5, testenum::value6, testenum::value7>;

static_assert(test_enum_tuple::count == magic_enum::enum_count<testenum>());
static_assert(enum_tuple_element<1, test_enum_tuple>::enum_value == magic_enum::enum_value<testenum>(1));
static_assert(enum_tuple_index<test_enum_tuple, testenum::value2>() == 1);
static_assert(std::is_same_v<decltype(make_enum_tuple<testenum>()), test_enum_tuple>);

struct enum_traits
{
    // static_assert();
};

using testenum_pair = std::pair<const testenum, const std::string_view>;
namespace
{
    using namespace std::literals::string_view_literals;
}
using test_enum_map = static_map<testenum, std::string_view, magic_enum::enum_count<testenum>()>;
constexpr const test_enum_map testenum_map{{{{testenum::value1, "value1"sv},
                                             {testenum::value2, "value2"sv},
                                             {testenum::value3, "value3"sv},
                                             {testenum::value4, "value4"sv},
                                             {testenum::value5, "value5"sv},
                                             {testenum::value6, "value6"sv},
                                             {testenum::value7, "value7"sv}}}};

using inv_test_enum_map = static_map<std::string_view, testenum, magic_enum::enum_count<testenum>()>;
constexpr const inv_test_enum_map testenum_mapswitched{testenum_map.switch_key_value()};

template <typename T, std::size_t MapSize>
[[nodiscard]] constexpr std::enable_if_t<std::is_enum_v<T>, std::optional<T>>
from_string(const std::string_view sv, const static_map<std::string_view, T, MapSize> &sv_map) noexcept
{
    return sv_map.at(sv);
}

static_assert(from_string("value2"sv, testenum_mapswitched).value() == testenum::value2);

template <typename T, T val, const char *U>
struct enum_name
{
    static_assert(std::is_enum_v<T>);
    using type = T;
    constexpr static type value() { return val; };
    static constexpr std::string_view name{U};
};

constexpr const char value1_str[] = "value1";
enum_name<testenum, testenum::value1, value1_str> myname;

static_assert(std::is_same_v<testenum, decltype(testenum::value1)>);

template<typename EnumType, template <EnumType> typename EnumTypeSelector>
struct enum_type_mapper {
    using enum_type = EnumType;

    template<typename EnumType,EnumType value, template <EnumType> typename EnumTypeSelector>
    using type = EnumTypeSelector<value>;

    template<>
    static constexpr decltype(auto) make_variant();

    template<>
    static constexpr make_variant();

    using variant_type = std::variant<>;
};
