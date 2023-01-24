#include "vpn-core.h"

#include <atomic>            // atomic<int>
#include <cstdlib>           // EXIT_FAILURE
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

using std::cerr; using std::endl; using std::move; using std::jthread;

extern "C" int tun_alloc(void);  // defined in vpn/vpn-linux-tun.c
extern "C" int setip(int const fd, char const *const str_dev, char const *const str_ip);

inline void last_words_exit(char const *const p)
{
    cerr << p << endl;
    exit(EXIT_FAILURE);
}

namespace VPN {

std::atomic<int> g_fd_listening_SOCKS{-1};

jthread g_thread_tun2socks;

static void ThreadEntryPoint_VPN_ListenToTun(std::stop_token st, int const tun_fd);

void Enable(void)
{
    void *const h = ::dlopen("./libtun2socks.so", RTLD_NOW);

    if ( nullptr == h ) last_words_exit("Could not load shared library libtun2socks.so");

    int (*tun2socks_main)(int,char**) = reinterpret_cast<int (*)(int,char**)>(   dlsym(h,"main")   );

    if ( nullptr == tun2socks_main ) last_words_exit("Could not get address of 'main' inside libtun2socks.so");

    int const tun_fd = tun_alloc();  /* tun interface */

    if ( tun_fd < 0 ) last_words_exit("Could not create and open tun device for VPN");

    // And now start separate thread for tun2socks

    static jthread dummy([tun_fd](){
        printf(" - - - dummy thread sleeping for 5 secs. . . - - -\n");
        sleep(3);
        printf(" - - - dummy thread setting IP address and bringing up - - -\n");
        setip(tun_fd,"tun0","10.10.10.1");
    });

    g_thread_tun2socks = jthread([tun2socks_main](void)->void
      {
          char *cmdline[] = {
              "badvpn-tun2socks",
              "--tundev", "tun0",
              "--netif-ipaddr", "10.10.10.2",
              "--netif-netmask", "255.255.255.0",
              "--socks-server-addr", "127.0.0.1:5555",
              nullptr,
              // The environment variables should be here
              nullptr,
              nullptr,
          };

          cerr << "VPN: Waiting for SOCKS listening port to open. . ." << endl;
          g_fd_listening_SOCKS.wait(-1);  // Wait for SOCKS to start listening
          cerr << "VPN: SOCKS listening port is now open, starting tun2socks. . ." << endl;
          int const retval = tun2socks_main(9, cmdline);
          cerr << "VPN: tun2socks finished with return value " << retval << endl;
      });

    //g_thread_vpn_tun.join();
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

}  // close namespace 'VPN'

extern "C" void VPN_Enable(void) noexcept { try { VPN::Enable(); } catch(...) {} }
extern "C" bool VPN_ParseNetworks(int const a, int const b) noexcept { try { return VPN::ParseNetworks(a,b); } catch(...) { return false; } }

extern "C" void VPN_Notify_SOCKS_Is_Listening(int const fd);
extern "C" void VPN_Notify_SOCKS_Is_Listening(int const fd)
{
    VPN::g_fd_listening_SOCKS = fd;
    VPN::g_fd_listening_SOCKS.notify_all();
}
