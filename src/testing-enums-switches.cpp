#include "testing-enums-switches.hpp"

#include <cstdio>
#include <iostream>

template <testenum value>
struct testenum_functor
{
    int operator()(int a, double b)
    {
        const auto res = a + (int) b + (int) value;
        std::cout << "Value: " << (int) value << std::endl;
        std::cout << "a: " << a << std::endl;
        std::cout << "b: " << b << std::endl;
        std::cout << "Res: " << res << std::endl;
        return res + 10;
    }
};
template <>
struct testenum_functor<testenum::value3>
{
    int operator()(int a, double b)
    {
        std::cout << "BLA: " << (int) testenum::value3 << std::endl;
        return 5;
    }
};
struct default_functor
{
    int operator()(int, double)
    {
        std::cout << "default case" << std::endl;
        return 0;
    }
};

int main()
{
    int a          = -5;
    double b       = 5.0;
    int input      = 0;
    testenum value = testenum::value5;
    std::cout << "Value:" << (int) value << std::endl;
    std::cout << "Expecting Input:" << std::endl;
    std::cin >> input;
    std::cout << "Input " << input << std::endl;
    value = static_cast<testenum>(input);
    std::cout << "Value:" << (int) value << std::endl;
    auto res = enum_switch::run<testenum, default_functor, testenum_functor>(value, a, b);
    std::cout << "Switch:" << res << std::endl;
    std::puts("Hello testing-grounds!");
    return 0;
}
