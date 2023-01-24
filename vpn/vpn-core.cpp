#include "vpn-core.h"

#include <cstdlib>           // EXIT_FAILURE
#include <iostream>          // cout, endl
#include <vector>
#include <utility>           // move
#include <thread>            // jthread
#include <stop_token>        // stop_token
#include <sstream>           // ostringstream

#include <unistd.h>          // read, close

#include "from_dynamo/frame.h"     // OFFSET_IP_SRC
#include "from_dynamo/checksum.h"  // IsValidIP4

#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio.hpp>

using std::cout; using std::endl; using std::move; using std::jthread;

extern "C" int tun_alloc(void);  // defined in vpn/vpn-linux-tun.c

inline void last_words_exit(char const *const p)
{
    cout << p << endl;
    exit(EXIT_FAILURE);
}

namespace VPN {

jthread g_thread_vpn_tun;

static void ThreadEntryPoint_VPN_ListenToTun(std::stop_token st, int const tun_fd);

void Enable(void)
{
    int const tun_fd = tun_alloc();  /* tun interface */

    if ( tun_fd < 0 )
    {
        last_words_exit("Could not create and open tun device for VPN");
    }

    try { g_thread_vpn_tun = jthread(ThreadEntryPoint_VPN_ListenToTun, tun_fd); }
    catch (...)
    {
        last_words_exit("Could not spawn a new thread to listen to the TUN device");
    }

    g_thread_vpn_tun.join();
}

bool ParseNetworks(int const a, int const b)
{
    // This is just preliminary test code to see if
    // the SSH client program called 'dbclient' links
    // properly with the libstdc++

    std::vector<int> v;

    for (int i = 0u; i != a; ++i) v.emplace_back(b);

    return v.front() == v.back();
}

static void ThreadEntryPoint_VPN_ListenToTun(std::stop_token st, int const tun_fd)
{
    // Before reading from the tun device, listen on port 443
    // for a connection from the remote server

    using namespace boost::asio;
    using ip::tcp;
    io_service my_io_service;
    tcp::acceptor my_acceptor(my_io_service, tcp::endpoint(tcp::v4(), 443u));
    tcp::socket my_socket(my_io_service);
    cout << "Waiting for a connection to be made. . ." << endl;
    my_acceptor.accept(my_socket);
    cout << "Connection established." << endl;

    char unsigned buffer[65535u];

    std::ostringstream oss;

    /* Now read data coming from the kernel */
    for ( ; ; oss.clear() )
    {
        /* Note that "buffer" should be at least the MTU size of the interface, eg 1500 bytes */
        int const nread = read(tun_fd,buffer,sizeof buffer);

        if ( nread < 0 )
        {
            close(tun_fd);
            last_words_exit("Error encountered when reading from tun device");
        }

        /* Do whatever with the data */
        oss << "Read " << nread << " bytes from TUN device:" << endl;

        if ( !IsValidIP4(buffer,buffer + nread) )
        {
            oss << "Invalid IPv4 Packet [";

            for ( int i = 0; i < nread; ++i )
            {
                static char const hex[] = "0123456789abcdef";
                oss << hex[ 0xF & (buffer[i] >> 4u) ]
                     << hex[ 0xF & (buffer[i] >> 0u) ];
            }

            oss << "]" << endl;
            continue;
        }

        uint32_t ip_src = Get32(buffer + OFFSET_IP_SRC),
                 ip_dst = Get32(buffer + OFFSET_IP_DST);

        using boost::asio::ip::address_v4;

        oss << "    From " << address_v4(ip_src).to_string() << " to " << address_v4(ip_dst).to_string();

        uint8_t const *p_trp = nullptr;

        switch ( buffer[OFFSET_IP_PROTO] ) /* This is OK coz it's 8-Bit */
        {
        case 0x01: oss << " - ICMP"; break;
        case 0x06:

            oss << " - TCP";

            if ( !IsValidIP4_TCP(buffer,buffer + nread) )
            {
                oss << " - invalid segment" << endl;
                continue;
            }

            /* Get a pointer to the start of the segment */
            p_trp = buffer + GetIP4HeaderLen(buffer);

            oss << " from port " << Get16(p_trp+OFFSET_TCP_SRC) << " to port " << Get16(p_trp+OFFSET_TCP_DST);
            break;

        case 0x11:

            oss << " - UDP";

            if ( !IsValidIP4_UDP(buffer,buffer + nread) )
            {
                cout << " - invalid datagram" << endl;
                continue;
            }

            /* Get a pointer to the start of the datagra */
            p_trp = buffer + GetIP4HeaderLen(buffer);

            oss << " from port " << Get16(p_trp+OFFSET_UDP_SRC) << " to port " << Get16(p_trp+OFFSET_UDP_DST);
            break;
        }

        oss << endl;
        cout << oss.rdbuf();
        boost::asio::write( my_socket, boost::asio::buffer( oss.str() ) );
    }
}

}  // close namespace 'VPN'

extern "C" void VPN_Enable(void) noexcept { try { VPN::Enable(); } catch(...) {} }
extern "C" bool VPN_ParseNetworks(int const a, int const b) noexcept { try { return VPN::ParseNetworks(a,b); } catch(...) { return false; } }

#include <sstream>
