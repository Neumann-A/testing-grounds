#pragma once

#include <algorithm>
#include <array>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <magic_enum.hpp>

#ifdef _HAS_CXX20
#pragma message("HASCXX20")
#endif
//https://www.youtube.com/watch?v=INn3xa4pMfg

#include <MyCEL/basics/templatehelpers.h>
#include <MyCEL/basics/enumhelpers.h>
#include <MyCEL/basics/concepts.hpp>
#include <MyCEL/types/static_map.hpp>


using namespace MyCEL;

// Helper concept
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////

template <const auto &, template <auto...> typename, typename>
struct unfold_array_on_type;
template <typename T, std::size_t N, template <T...> typename Output_t, const std::array<T, N> &Input, std::size_t... I>
struct unfold_array_on_type<Input, Output_t, std::index_sequence<I...>>
{
    using result_type = Output_t<Input[I]...>;
};
template <typename T, std::size_t N, template <T...> typename Output_t, const std::array<T, N> &Input, std::size_t... I>
using unfold_array_on_type_result_t =
    typename unfold_array_on_type<Input, Output_t, std::index_sequence<I...>>::result_type;

template <const auto &Input, template <auto...> typename Output_t>
struct apply_array_on_type
{
    using result_type =
        typename unfold_array_on_type<Input, Output_t, std::make_index_sequence<Input.size()>>::result_type;
};
template <typename T, std::size_t N, template <T...> typename Output_t, const std::array<T, N> &Input>
struct apply_array_on_type<Input, Output_t>
{
    using result_type = typename unfold_array_on_type<Input, Output_t, std::make_index_sequence<N>>::result_type;
};
template <MyCEL::std_array auto Input,
          template <typename std::remove_reference_t<decltype(Input)>::value_type...> typename Output_t>
using apply_array_on_type_result_t = typename apply_array_on_type<Input, Output_t>::result_type;

template <const auto &Input, template <typename...> typename Output,
          template <typename std::decay_t<decltype(Input)>::value_type...> typename Mapper, std::size_t... Is>
constexpr auto apply_input_via_mapper_to_output_impl(std::index_sequence<Is...>)
    -> Output<typename Mapper<Input.operator[](Is)>::type...>;

template <const auto &Input, template <typename...> typename Output,
          template <typename std::decay_t<decltype(Input)>::value_type...> typename Mapper>
constexpr auto apply_input_via_mapper_to_output() -> decltype(
    apply_input_via_mapper_to_output_impl<Input, Output, Mapper>(std::make_index_sequence<std::size(Input)>()));

template <template <typename...> typename Output, const auto &Input,
          template <std::remove_cvref_t<decltype(get<0>(Input))>...> typename Mapping, std::size_t... Is>
constexpr auto map_values_to_types_impl(std::index_sequence<Is...>)
    -> Output<typename Mapping<get<Is>(Input)>::type...>;
template <template <typename...> typename Output, const auto &Input,
          template <std::remove_cvref_t<decltype(get<0>(Input))>...> typename Mapping>
constexpr auto map_values_to_types()
    -> decltype(map_values_to_types_impl<Output, Input, Mapping>(std::make_index_sequence<size(Input)>()));
template <template <typename...> typename Output, const auto &Input,
          template <std::remove_cvref_t<decltype(get<0>(Input))>...> typename Mapping>
using map_values_to_types_t = decltype(map_values_to_types_impl<Output, Input, Mapping>((std::make_index_sequence<size(Input)>())));

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

template <testenum value>
struct testenum_type_mapper
{
    using type = double;
    //using template_arg = value;
};
template <>
struct testenum_type_mapper<testenum::value4>
{
    using type = int;
    //using template_arg = testenum::value4;
};

template< template <auto...> typename T>
struct identity_mapping
{
    template<auto U>
    using type = T<U>;
};

constexpr const std::array<testenum, 7> testenum_values = {testenum::value1, testenum::value2, testenum::value3,
                                                           testenum::value4, testenum::value5, testenum::value6,
                                                           testenum::value7};
//constexpr const std::array<testenum,  magic_enum::enum_count<testenum>()> testenum_values = magic_enum::enum_values<testenum>();

template <testenum... Values>
using testenum_variant_helper_t = MyCEL::enum_variant_creator_t<testenum, testenum_type_mapper, Values...>;
using test_nttp = apply_nttp_t<testenum_values,testenum_variant_helper_t>;
using testenum_variant3_t = map_values_to_types_t<std::variant, testenum_values, testenum_type_mapper>;

using testenum_variant4_t = map_values_to_types_t<std::variant, testenum_values, identity_mapping<testenum_type_mapper>::type>;

static_assert(std::is_same_v<testenum_variant3_t,test_nttp>);

using testenum_variant_t        = typename apply_array_on_type<testenum_values, testenum_variant_helper_t>::result_type;
using testenum_variant2_t =
    decltype(apply_input_via_mapper_to_output<testenum_values, std::variant, testenum_type_mapper>());




static_assert(std::is_same_v<typename std::variant_alternative_t<3, testenum_variant_t>, int>);
static_assert(!std::is_same_v<typename std::variant_alternative_t<4, testenum_variant_t>, int>);
static_assert(std::is_same_v<typename std::variant_alternative_t<4, testenum_variant_t>, double>);
static_assert(std::is_same_v<typename std::variant_alternative_t<4, testenum_variant2_t>, double>);

using test_enum_tuple = enum_tuple<testenum, testenum::value1, testenum::value2, testenum::value3, testenum::value4,
                                   testenum::value5, testenum::value6, testenum::value7>;
using test_enum_tuple2 = MyCEL::make_enum_tuple_type<testenum>;
static_assert(test_enum_tuple::count == magic_enum::enum_count<testenum>());
static_assert(enum_tuple_element<1, test_enum_tuple>::enum_value == magic_enum::enum_value<testenum>(1));
static_assert(enum_tuple_index<test_enum_tuple, testenum::value2>() == 1);
static_assert(std::is_same_v<decltype(make_enum_tuple<testenum>()), test_enum_tuple>);

static_assert(std::is_same_v<test_enum_tuple2,test_enum_tuple>);

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

static_assert(testenum_map.get_key_array()[3]==testenum::value4);

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
[[maybe_unused]] enum_name<testenum, testenum::value1, value1_str> myname;

static_assert(std::is_same_v<testenum, decltype(testenum::value1)>);

//From https://godbolt.org/z/b7zKr4
//

// template<typename EnumType, template <EnumType> typename EnumTypeSelector>
// struct enum_type_mapper {
//     using enum_type = EnumType;

//     template<typename EnumType,EnumType value, template <EnumType> typename EnumTypeSelector>
//     using type = EnumTypeSelector<value>;

//     template<std::size_t index, typename VariantIn, typename AppendType>
//     static constexpr decltype(auto) make_variant_impl();
//     template<>
//     static constexpr decltype(auto) make_variant();

//     template<>
//     static constexpr make_variant();

//     using variant_type = std::variant<>;
// };

// constexpr std::array wheels = {1, 2, 3, 4, 6};

// template <auto N>
// requires(std::find(begin(wheels), end(wheels), N) != end(wheels)) // if you really want a concept this works
// struct Vehicle {
//     static constexpr auto n_wheels = N;
// };

// template <const auto& W, std::size_t... Is>
// auto vehicle_variants(std::index_sequence<Is...>) -> std::variant<Vehicle<W.operator[](Is)>...>;

// template <const auto& W>
// auto vehicle_variants() -> decltype(vehicle_variants<W>(std::make_index_sequence<size(W)>()));

// using Vehicles = decltype(vehicle_variants<wheels>());

// using Garage = std::vector<Vehicles>;

template <typename EnumType>
struct enum_info
{
    static constexpr auto values = magic_enum::enum_values<EnumType>();
};
