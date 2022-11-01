
#include <string_view>
#include <asio.hpp>
#include <asio/ip/address_v6.hpp>
#include <asio/ip/resolver_base.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <memory>
#include <iostream> 
#include <algorithm>

#include <WinSock2.h>
#include <iphlpapi.h>
#include <ws2ipdef.h>

#pragma comment(lib, "IPHLPAPI.lib")


using namespace std::literals::string_view_literals;

const std::array nodelist{"node02"sv,
                          "heineken"sv,
                          "node03"sv};

static auto get_local_adapter_adresses() {
    std::vector<asio::ip::address> res;
    typedef IP_ADAPTER_ADDRESSES *IPAdresses;
    unsigned long buf_size = 0;
    auto err = GetAdaptersAddresses(AF_INET,
                                        GAA_FLAG_INCLUDE_PREFIX | 
                                        GAA_FLAG_SKIP_ANYCAST | 
                                        GAA_FLAG_SKIP_DNS_SERVER ,
                                        NULL, NULL,
                                        &buf_size);

    if(err != ERROR_BUFFER_OVERFLOW) //Should overflow since buf_size = 0
        return res;

    auto ifaddrs = (IPAdresses) new char[buf_size];
    err = GetAdaptersAddresses(AF_INET,
                                          GAA_FLAG_INCLUDE_PREFIX | 
                                          GAA_FLAG_SKIP_DNS_SERVER ,
                                          NULL, ifaddrs,
                                          &buf_size);

    if(err != ERROR_SUCCESS)
        fmt::print("Error while getting Adapter Adresses. Error Code '{}'",err);

    for(auto addr = ifaddrs; addr != 0; addr = addr->Next) {
        if (addr->OperStatus != IfOperStatusUp) continue;

                // Find the first IPv4 address
        if (addr->Ipv4Enabled) {
            for (auto *uaddr = addr->FirstUnicastAddress; uaddr != nullptr; uaddr = uaddr->Next) {
                const auto sock_in = reinterpret_cast<sockaddr_in&>(*(uaddr->Address.lpSockaddr));

                if (sock_in.sin_family != AF_INET) continue;

                const auto ipv4 = ntohl(sock_in.sin_addr.S_un.S_addr);
                res.push_back(asio::ip::make_address_v4(ipv4));
            }
        }
        if (addr->Ipv6Enabled) {
            for (auto *uaddr = addr->FirstUnicastAddress; uaddr != nullptr; uaddr = uaddr->Next) {
                auto sock_in = reinterpret_cast<sockaddr_in6&>(*(uaddr->Address.lpSockaddr));
                if (sock_in.sin6_family != AF_INET6) continue;
                    asio::ip::address_v6::bytes_type ipv6;
                    for( int i = 0; i < 16; i++ )
                        ipv6[i] = sock_in.sin6_addr.u.Byte[i];
                    res.push_back(asio::ip::make_address_v6(ipv6,sock_in.sin6_scope_id));
                }
            }
    };
    delete[]((char *)ifaddrs);
    return res;
}


static int get_first_local()
{
    using asio::ip::tcp;
    auto localadresses = get_local_adapter_adresses();
    int res = -1;
    int counter = 0;
    // Remove loopbacks
    localadresses.erase(std::remove_if(localadresses.begin(),
                                            localadresses.end(),
                                            [](auto val){return val.is_loopback();}));

    for(auto l_adr : localadresses) {
        fmt::print("\nIpv4: {}, Ipv6 {}\n", l_adr.is_v4(), l_adr.is_v6());
        fmt::print("Details: {}\n", l_adr.to_string());
        fmt::print("Loopback: {}\n", l_adr.is_loopback());
    }
    for(auto node : nodelist) {
        fmt::print("\nNode: {}\n", node);
        asio::io_service io_service;
        asio::ip::tcp::resolver resolver(io_service);
        auto endpoints = resolver.resolve(node,"");
        for(auto ep : endpoints) {
            fmt::print("Endpoint: {}\n",ep.endpoint().address().to_string());
            for(auto l_adr : localadresses) {
                if (l_adr == ep.endpoint().address())
                {
                    fmt::print("Found local endpoint!\n");
                    res = counter;
                }
            }
        }
        ++counter;
    }
    return res;
};

int main()
{
    fmt::print("Local network name in nodelist is: {}",nodelist[get_first_local()]);
    return 0;
};
