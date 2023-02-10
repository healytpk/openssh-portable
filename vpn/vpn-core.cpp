#include "vpn-core.h"

#include <atomic>            // atomic<int>
#include <cstdlib>           // EXIT_FAILURE
#include <cstdio>            // sprintf
#include <iostream>          // cout, endl
#include <vector>
#include <utility>           // move
#include <thread>            // jthread
#include <stop_token>        // stop_token
#include <sstream>           // ostringstream

#include <unistd.h>          // read, close
#include <dlfcn.h>           // dlopen, dlclose

#include "from_dynamo/frame.h"     // OFFSET_IP_SRC
#include "from_dynamo/checksum.h"  // IsValidIP4

#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio.hpp>

using std::cout; using std::cerr; using std::endl; using std::move; using std::jthread;

// The next three are defined in vpn/vpn-linux-tun.c
extern "C" int g_fd_tun;
extern "C" char g_str_tun_ifnam[IFNAMSIZ];
extern "C" void tun_alloc(void);

inline void last_words_exit(char const *const p)
{
    cerr << p << endl;
    exit(EXIT_FAILURE);
}

extern "C" int badvpn_main(int,char**);
extern "C" int busybox_route_main(int,char**);
extern "C" void busybox_bb_displayroutes(int noresolve, int netstatfmt) __attribute__((regparm(3),stdcall));

namespace VPN {

extern "C" long unsigned g_ip_address_of_remote_SSH_server = 0u;  // Stored in NetworkByteOrder (i.e. BigEndian) even on LittleEndian machines

std::atomic<int> g_fd_listening_SOCKS{-1};

jthread g_thread_tun2socks;

static void Deal_With_Routing_Table(void)
{
    static_assert( 8u == CHAR_BIT, "Cannot handle 16-Bit char's or whatever size they are" );

    char unsigned const *const p = static_cast<char unsigned const*>(static_cast<void const*>(&g_ip_address_of_remote_SSH_server));

    // g_ip_address_of_remote_SSH_server is always in NetworkByteOrder (i.e. BigEndian) even on LittleEndian machines.
    // Strangely though, on machines where 8 == sizeof(unsigned long), the 4 bytes we want are at the beginning rather
    // than at the end, i.e. it's stored as [a][b][c][d][0][0][0][0] instead of [0][0][0][0][a][b][c][d]
    cout << "Must create unique entry in routing table for "
         << static_cast<unsigned>(p[0]) << "."
         << static_cast<unsigned>(p[1]) << "."
         << static_cast<unsigned>(p[2]) << "."
         << static_cast<unsigned>(p[3]) << endl;

    cout << "Here's how your routing table currently looks:\n";

    char *cmdline[] = {
        "route",
        "-n",
        nullptr,
        // The environment variables should be here
        nullptr,
        nullptr,
    };

    busybox_bb_displayroutes(0x0fffu, 0);

    cout << "Do you wish to use 10.10.10.0/24 for the VPN?\n";
    int i;
    //std::cin >> i;
}

void Enable(void)
{
    Deal_With_Routing_Table();

    cerr << "Creating TUN device. . .\n";

    tun_alloc();

    if ( g_fd_tun < 0 ) last_words_exit("Could not create and open tun device for VPN");

    cerr << "TUN device successfully created.\n";
}

static int setip(int const fd, char const *const str_dev, char const *const str_ip);

static void Start(std::stop_token)
{
    cerr << "=============== Spawn new thread : VPN Thread ===============\n";

    cerr << "VPN Thread: Setting IP address of TUN device. . .\n";
    setip(g_fd_tun,"tun0","10.10.10.1");
    cerr << "VPN Thread: IP address of TUN device is now set.\n";

    char *cmdline[] = {
        "badvpn-tun2socks",
        "--tundev", g_str_tun_ifnam,
        "--netif-ipaddr", "10.10.10.2",
        "--netif-netmask", "255.255.255.0",
        "--socks-server-addr", "127.0.0.1:5555",
        "--udpgw-remote-server-addr", "127.0.0.1:7300",
        nullptr,
        // The environment variables should be here
        nullptr,
        nullptr,
    };

    dup2(fileno(stderr),fileno(stdout));  // cout becomes cerr
    busybox_bb_displayroutes(0x0fffu, 0);
    cerr << "VPN: Waiting 5 seconds and then entering 'main' of tun2socks. . .";
    cerr << "5 ... "; sleep(1); cerr << "4 ... "; sleep(1); cerr << "3 ... "; sleep(1); cerr << "2 ... "; sleep(1); cerr << "1 ..."; sleep(1); cerr << endl;
    busybox_bb_displayroutes(0x0fffu, 0);
    int const retval = badvpn_main(11, cmdline);
    cerr << "VPN: 'main' of tun2socks has returned with value " << retval << endl;
    cerr << "=============== Thread finished : VPN Thread ===============\n";
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

int setip(int const fd, char const *const str_dev, char const *const str_ip)
{
    int retval = -1;

    struct ::ifreq ifr = {0};
    struct ::sockaddr_in addr = {0};

    addr.sin_family = AF_INET;
    //if ( 1 != inet_pton(addr.sin_family, str_ip, &addr.sin_addr) ) goto End;

#if 0
    std::strncpy(ifr.ifr_name, str_dev, IFNAMSIZ);


    int const s = ::socket(addr.sin_family, SOCK_DGRAM, 0);
    if ( -1 == s ) goto End;

    ifr.ifr_addr = *(struct ::sockaddr*)&addr;
    ((struct ::sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = htonl(0xFFFFFF00);

    if ( -1 == ::ioctl(s, SIOCSIFADDR   , &ifr) ) goto End;
    if ( -1 == ::ioctl(s, SIOCSIFNETMASK, &ifr) ) goto End;

    if ( -1 == ::ioctl(s, SIOCGIFFLAGS, &ifr) ) goto End;
    ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
    if ( -1 == ::ioctl(s, SIOCSIFFLAGS, &ifr) ) goto End;

    //wipe_out_ipv6(fd,str_dev);

    retval = fd;


#else

    int const s = -1;

    retval = fd;

    char buf[256u];

    //sleep(5);

    //sprintf(buf,"ifconfig %s 10.10.10.1 netmask 255.255.255.0",str_dev);
    //system(buf);
    std::system("sudo ifconfig tun0 10.10.10.1 netmask 255.255.255.0");

    //sleep(5);

    //sprintf(buf,"ifconfig %s up",str_dev);
    //system(buf);
    std::fprintf(stderr, "Bringing tun0 up. . . .");
    std::system("sudo ifconfig tun0 up");
    std::fprintf(stderr, "tun0 is now up");

    char str_ip_remote_SSH_server[64u];
    char unsigned const *const p = (char unsigned const*)&g_ip_address_of_remote_SSH_server;
    std::sprintf(str_ip_remote_SSH_server,"%u.%u.%u.%u", (unsigned)p[0], (unsigned)p[1], (unsigned)p[2], (unsigned)p[3]);

    char *cmdline[] = {
        "route",
        "add", "-net", str_ip_remote_SSH_server, "netmask", "255.255.255.255",
        "gw", "192.168.58.2",
        "metric", "5",
        "dev", "ens33",
        nullptr,
        // The environment variables should be here
        nullptr,
        nullptr,
    };

    //busybox_route_main(12,cmdline);

    std::system("route del -net 0.0.0.0 netmask 0.0.0.0 gw `route -n | grep \"^0\\.0\\.0\\.0\" | xargs | cut -d ' ' -f 2`");

    //sprintf(buf,"route add -net 0.0.0.0 netmask 0.0.0.0 gw 10.10.10.2 metric 6 dev %s",str_dev);
    //system(buf);
    std::system("route add -net 0.0.0.0 netmask 0.0.0.0 gw 10.10.10.2 metric 6 dev tun0");
#endif

End:
    if ( -1 != s ) close(s);
    return retval;
}

}  // close namespace 'VPN'

extern "C" void VPN_Enable(void) noexcept { try { VPN::Enable(); } catch(...) {} }
extern "C" bool VPN_ParseNetworks(int const a, int const b) noexcept { try { return VPN::ParseNetworks(a,b); } catch(...) { return false; } }

extern "C" void VPN_Notify_SOCKS_Is_Listening(int const fd);
extern "C" void VPN_Notify_SOCKS_Is_Listening(int const fd)
{
    VPN::g_fd_listening_SOCKS = fd;
    VPN::g_fd_listening_SOCKS.notify_all();

    static std::jthread mythread(VPN::Start);
}
