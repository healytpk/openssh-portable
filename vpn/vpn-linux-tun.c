#ifdef __cplusplus
#error You must use a C compiler to compile this file (not a C++ compiler)
#include "Nonexistent_header_file_see_previous_error"
#endif

#define _GNU_SOURCE     /* Allows us to use RTLD_NEXT with dlsym */
#include <stdbool.h>         // bool, true, false
#include <stdio.h>           // sprintf
#include <stdlib.h>          // system, abort
#include <string.h>          // strncpy, memset
#include <sys/ioctl.h>       // ioctl
#include <fcntl.h>           // open
#include <linux/if.h>        // ifreq
#include <linux/if_tun.h>
#include <unistd.h>          // close
#include <sys/socket.h>      // socket
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>       // htonl
#include <dlfcn.h>           // dlsym

#define nullptr (0)

int g_fd_tun = -1;
char g_str_tun_ifnam[IFNAMSIZ] = {0};

struct Dynamic_Function_Pointers {
	int (*open )(const char *, int, ...);
	int (*ioctl)(const char *, int, ...);
};

static struct Dynamic_Function_Pointers g_real_funcs = { nullptr };  /* All function pointers start off as null */

void Set_Function_Pointers(void)
{
    static bool already_done = false;

    if ( already_done ) return;

    already_done = true;

    g_real_funcs.open  = dlsym(RTLD_NEXT, "open" );
    g_real_funcs.ioctl = dlsym(RTLD_NEXT, "ioctl");

    for ( unsigned i = 0u; i != (sizeof g_real_funcs / sizeof(void(*)(void))); ++i )
    {
        if ( nullptr == ((void(**)(void))&g_real_funcs)[i] ) abort();
    }
}

int open(char const *const pathname, int const flags, ...)
{
    if ( 0 == strcmp(pathname,"/dev/net/tun") ) return g_fd_tun;

    Set_Function_Pointers();

    void *const ret = __builtin_apply(g_real_funcs.open, __builtin_apply_args(), 1000);

    __builtin_return(ret);
}

int ioctl(int fd, unsigned long request, ...)
{
    if ( TUNSETIFF == request ) return 1;

    Set_Function_Pointers();

    void *const ret = __builtin_apply(g_real_funcs.ioctl, __builtin_apply_args(), 1000);

    __builtin_return(ret);
}

int setip(int const fd, char const *const str_dev, char const *const str_ip)
{
    int retval = -1;

    struct ifreq ifr = {0};
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET;
    //if ( 1 != inet_pton(addr.sin_family, str_ip, &addr.sin_addr) ) goto End;

#if 0
    strncpy(ifr.ifr_name, str_dev, IFNAMSIZ);


    int const s = socket(addr.sin_family, SOCK_DGRAM, 0);
    if ( -1 == s ) goto End;

    ifr.ifr_addr = *(struct sockaddr*)&addr;
    ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = htonl(0xFFFFFF00);

    if ( -1 == ioctl(s, SIOCSIFADDR   , &ifr) ) goto End;
    if ( -1 == ioctl(s, SIOCSIFNETMASK, &ifr) ) goto End;

    if ( -1 == ioctl(s, SIOCGIFFLAGS, &ifr) ) goto End;
    ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
    if ( -1 == ioctl(s, SIOCSIFFLAGS, &ifr) ) goto End;

    //wipe_out_ipv6(fd,str_dev);

    retval = fd;


#else

    int const s = -1;

    retval = fd;

    char buf[256u];

    //sleep(5);

    //sprintf(buf,"ifconfig %s 10.10.10.1 netmask 255.255.255.0",str_dev);
    //system(buf);
    system("sudo ifconfig tun0 10.10.10.1 netmask 255.255.255.0");

    //sleep(5);

    //sprintf(buf,"ifconfig %s up",str_dev);
    //system(buf);
    fprintf(stderr, "Bringing tun0 up. . . .");
    system("sudo ifconfig tun0 up");
    sleep(5);
    fprintf(stderr, "tun0 is now up");

    system("route add -net `nslookup virjacode.com | grep Address | cut -d ' ' -f2 | tail -1` netmask 255.255.255.255 gw `route -n | grep \"^0\\.0\\.0\\.0\" | xargs | cut -d ' ' -f 2` metric 5 dev ens33");
    system("route del -net 0.0.0.0 netmask 0.0.0.0 gw `route -n | grep \"^0\\.0\\.0\\.0\" | xargs | cut -d ' ' -f 2`");

    //sprintf(buf,"route add -net 0.0.0.0 netmask 0.0.0.0 gw 10.10.10.2 metric 6 dev %s",str_dev);
    //system(buf);
    system("route add -net 0.0.0.0 netmask 0.0.0.0 gw 10.10.10.2 metric 6 dev tun0");
#endif

End:
    if ( -1 != s ) close(s);
    return retval;
}

void tun_alloc(void)
{
    Set_Function_Pointers();

    if ( (g_fd_tun = g_real_funcs.open("/dev/net/tun", O_RDWR)) < 0 ) return;

    struct ifreq ifr = {0};
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    int err;

    if ( (err = g_real_funcs.ioctl(g_fd_tun, TUNSETIFF, &ifr)) < 0 )
    {
        close(g_fd_tun);
        g_fd_tun = err;
    }

    strcpy(g_str_tun_ifnam, ifr.ifr_name);

    if ( 0 != memcmp("tun",g_str_tun_ifnam,3u) )
    {
        close(g_fd_tun);
        g_fd_tun = -1;
        fprintf(stderr, "Cannot determine name of new TUN device. Bailing out.\n");
    }
}
