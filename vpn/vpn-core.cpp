#include "vpn-core.h"

#include <cassert>           // assert
#include <cstdlib>           // exit, EXIT_FAILURE
#include <cstdio>            // sprintf, fscanf, fopen, fclose
#include <utility>           // move
#include <atomic>            // atomic<int>
#include <vector>            // vector
#include <set>               // set
#include <algorithm>         // max
#include <sstream>           // stringstream
#include <thread>            // jthread
#include <stop_token>        // stop_token
#include <iostream>          // cout, cerr, endl

//#include <boost/asio/ip/address_v4.hpp>
//#include <boost/asio.hpp>

/* objects   */ using std::cout; using std::cerr; using std::endl;
/* functions */ using std::move;
/* types     */ using std::string; using std::string_view; using std::jthread;

// I've made sure the function on the next line returns uint32_t and not
// unsigned long. The value returned is in NetworkByte order (i.e. BigEndian)
extern "C" std::uint32_t inet_addr(char const*);

// The next five are defined in vpn/vpn-linux-tun.c
extern "C" int g_fd_tun;
extern "C" char g_str_tun_ifnam[257u];
extern "C" void tun_alloc(void);
extern "C" void set_ip(char const *const str_dev, char const *const str_ip, char const *const str_netmask);
extern "C" void bring_interface_up(char const *const str_dev);
extern "C" int is_interface_up(char const *const str_interface);
extern "C" void add_route(unsigned long const dest, unsigned long const netmask, unsigned long const gateway, char const *const str_dev, unsigned const metric);

// The next one is defined in channels.c
extern "C" long unsigned g_ip_address_of_remote_SSH_server = 0u;  // Stored in NetworkByteOrder (i.e. BigEndian) even on LittleEndian machines
extern "C" int badvpn_main(int,char**);

inline void last_words_exit(char const *const p)
{
    cerr << p << endl;
    std::exit(EXIT_FAILURE);
}

/* declaration */ extern "C" std::vector< std::pair<std::uint32_t,std::uint32_t> > g_VPN_excluded_routes;
/* definition  */            std::vector< std::pair<std::uint32_t,std::uint32_t> > g_VPN_excluded_routes;

namespace VPN {

struct RoutingTableEntry {
    unsigned metric;
    bool is_up;
    string str_gw, str_devname;
    string str_dest, str_netmask;  // For default gateway: 0.0.0.0/0

    //DefaultGatewayEntry(void) : metric(0u), str_ip(), is_up(false) {}

    bool operator<(RoutingTableEntry const &rhs) const
    {
        if (  is_up && !rhs.is_up ) return true;
        if ( !is_up &&  rhs.is_up ) return false;

        // If control reaches here, they're either both up or both down
        return metric < rhs.metric;
    }
};

struct Routing_Table_Summary {
    unsigned lowest_metric;
    unsigned lowest_metric_up;
    std::set<RoutingTableEntry> gws;
};

static Routing_Table_Summary get_entire_routing_table(bool only_get_default_gateways = false);

static inline Routing_Table_Summary get_default_gateways(void)
{
    return get_entire_routing_table(true);
}

static unsigned get_lowest_metric(Routing_Table_Summary const &rts)
{
    if ( rts.lowest_metric    >= 3u ) return rts.lowest_metric;
    if ( rts.lowest_metric_up >= 3u ) return rts.lowest_metric_up;

    return -1;
}

std::atomic<int> g_fd_listening_SOCKS{-1};

jthread g_thread_tun2socks;

void Enable(void)
{
    dup2(fileno(stderr),fileno(stdout));  // cout becomes cerr

    //Deal_With_Routing_Table();

    cerr << "Main Thread: Creating TUN device. . .\n";

    tun_alloc();

    if ( g_fd_tun < 0 ) last_words_exit("Main Thread: Could not create and open TUN device for VPN.");

    cerr << "Main Thread: TUN device successfully created.\n";
}

static void Start(std::stop_token)
{
    cerr << "=============== New thread spawned : VPN Thread ===============\n";

    cerr << "VPN Thread: Setting IP address of TUN device. . .\n";
    set_ip(g_str_tun_ifnam,"10.10.10.1", "255.255.255.0");
    cerr << "VPN Thread: IP address of TUN device is now set.\n";
    cerr << "VPN Thread: Bringing TUN device up. . .\n";
    bring_interface_up(g_str_tun_ifnam);
    cerr << "VPN Thread: Checking that the TUN device was brought up successfully. . .\n";
    if ( ! is_interface_up(g_str_tun_ifnam) ) last_words_exit("FATAL ERROR: TUN device isn't up.");
    cerr << "VPN Thread: TUN device is now up.\n";

    Routing_Table_Summary const rts = get_default_gateways();

    if ( rts.gws.empty() )
    {
        last_words_exit("Cannot find a default gateway on the routing table. Bailing out. . .");
    }

    unsigned const lowest_metric = get_lowest_metric(rts);

    if ( -1 == lowest_metric )
    {
        std::stringstream ss;
        ss << "No wiggle room on routing table for a lower metric. lowest_metric=" << rts.lowest_metric
           << ", lowest_metric_up=" << rts.lowest_metric_up << endl;
        last_words_exit( std::move(ss).str().c_str() );
    }

    cerr << "============================== Lowest Metric: " << lowest_metric << " ==================================" << endl;

    for ( auto &route : g_VPN_excluded_routes )
    {
        add_route(route.first,route.second,::inet_addr(rts.gws.cbegin()->str_gw.c_str()),"dummy",lowest_metric - 3u);
    }

    add_route(g_ip_address_of_remote_SSH_server,0xFFFFFFFFu,::inet_addr(rts.gws.cbegin()->str_gw.c_str()),"dummy",lowest_metric - 2u);
    add_route(                      0x00000000u,0x00000000u,::inet_addr("10.10.10.2"                    ),"dummy",lowest_metric - 1u);

    assert( 0u != g_local_ephemeral_port_for_SOCKS );

    string str_socks_server_addr{"127.0.0.1:"};
    str_socks_server_addr += std::to_string(g_local_ephemeral_port_for_SOCKS);

    char *cmdline[] = {
        "badvpn-tun2socks",
        "--tundev", g_str_tun_ifnam,
        "--netif-ipaddr", "10.10.10.2",
        "--netif-netmask", "255.255.255.0",
        "--socks-server-addr", const_cast<char*>(str_socks_server_addr.c_str()),
        "--udpgw-remote-server-addr", "127.0.0.1:7300",
        nullptr,
        // The environment variables should be here
        nullptr,
        nullptr,
    };

    cerr << "=============== About to start tun2socks to connect to SOCKS server on TCP " << str_socks_server_addr << "===============\n";

    //cerr << "5 ... "; sleep(1); cerr << "4 ... "; sleep(1); cerr << "3 ... "; sleep(1); cerr << "2 ... "; sleep(1); cerr << "1 ..."; sleep(1); cerr << endl;
    cerr << "VPN: Entering 'main' of tun2socks. . .";
    int const retval = badvpn_main(11, cmdline);
    cerr << "VPN: 'main' of tun2socks has returned with value " << retval << endl;
    cerr << "=============== Thread finished : VPN Thread ===============\n";
}

string ip_to_str(long unsigned const x)
{
    static_assert( 8u == CHAR_BIT, "Can't deal with 16-Bit char's or whatever size they are" );
    char unsigned const *const p = static_cast<char unsigned const *>(static_cast<void const*>(&x));
    std::ostringstream ss;
    ss << static_cast<unsigned>(p[0]) << "." << static_cast<unsigned>(p[1]) << "." << static_cast<unsigned>(p[2]) << "." << static_cast<unsigned>(p[3]);
    return std::move(ss).str();
}

static Routing_Table_Summary get_entire_routing_table(bool const only_get_default_routes)
{
    Routing_Table_Summary retval;
    retval.lowest_metric    = -1;
    retval.lowest_metric_up = -1;

    std::set<RoutingTableEntry> &defgws = retval.gws;

    char devname[64u], flags[16u];
    unsigned long d, g, m;
    int r;
    int flgs, ref, use, metric, mtu, win, ir;

    FILE *const fp = std::fopen("/proc/net/route", "r");

    if ( nullptr == fp ) throw std::runtime_error("File open error");

    /* Skip the first line. */
    r = std::fscanf(fp, "%*[^\n]\n");

    if ( r < 0 ) return retval;  // This will happen if routing table is empty

    for (; /* ever */ ;)
    {
        r = std::fscanf(fp, "%63s%lx%lx%X%d%d%d%lx%d%d%d\n",
                        devname, &d, &g, &flgs, &ref, &use, &metric, &m,
                        &mtu, &win, &ir);

        if ( 11 != r )
        {
            if ( (r < 0) && feof(fp) ) break; /* EOF with no (nonspace) chars read. */

            throw std::runtime_error("File read error");
        }

        /*
        if ( !(flgs & RTF_UP) ) continue; // Skip interfaces that are down
        */

        if ( only_get_default_routes && !((0u == d) && (0u == m)) ) continue;

        defgws.emplace(RoutingTableEntry{metric, flgs & 1u, ip_to_str(g), devname, ip_to_str(d), ip_to_str(m)});
        retval.lowest_metric = std::min(retval.lowest_metric, static_cast<unsigned>(metric));
        if ( flgs & 1u ) retval.lowest_metric_up = std::min(retval.lowest_metric_up, static_cast<unsigned>(metric));
    }

    fclose(fp);

    return retval;
}

}  // close namespace 'VPN'

extern "C" void VPN_Enable(void) noexcept { try { VPN::Enable(); } catch(...) {} }

extern "C" void VPN_Notify_SOCKS_Is_Listening(int const fd);
extern "C" void VPN_Notify_SOCKS_Is_Listening(int const fd)
{
    VPN::g_fd_listening_SOCKS = fd;
    VPN::g_fd_listening_SOCKS.notify_all();

    if ( 0u == VPN::g_local_ephemeral_port_for_SOCKS )
    {
        last_words_exit("Could not determine ephemeral TCP listening port for SOCKS proxy. Bailing out...");
    }

    static std::jthread mythread(VPN::Start);
}
