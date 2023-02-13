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
char g_str_tun_ifnam[257u] = {0};

int open_FOR_badvpn(char const *const pathname, int const flags, ...)
{
    if ( 0 == strcmp(pathname,"/dev/net/tun") ) return g_fd_tun;

    void *const ret = __builtin_apply(open, __builtin_apply_args(), 1000);

    __builtin_return(ret);
}

int ioctl_FOR_badvpn(int const fd, unsigned long const request, ...)
{
    if ( TUNSETIFF == request ) return 1;

    void *const ret = __builtin_apply(ioctl, __builtin_apply_args(), 1000);

    __builtin_return(ret);
}

void tun_alloc(void)
{
    if ( (g_fd_tun = open("/dev/net/tun", O_RDWR)) < 0 ) return;

    struct ifreq ifr = {0};
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    int err;

    if ( (err = ioctl(g_fd_tun, TUNSETIFF, &ifr)) < 0 )
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

void bring_interface_up(int const fd, char const *const str_dev)
{
    //fprintf(stderr, "Bringing virtual network interface '%s' up. . . .", str_dev);

    char buf[300u] = "ifconfig ";
    strcat(buf,str_dev);
    strcat(buf," up");
    system(buf);

    //fprintf(stderr, "Virtual network interface '%s' is now up", str_dev);
}

void set_ip(int const fd, char const *const str_dev, char const *const str_ip)
{
    struct ifreq ifr = {0};
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET;
    //if ( 1 != inet_pton(addr.sin_family, str_ip, &addr.sin_addr) ) goto End;

#if 0
    std::strncpy(ifr.ifr_name, str_dev, IFNAMSIZ);
    int const s = ::socket(addr.sin_family, SOCK_DGRAM, 0);
    if ( -1 == s ) goto End;
    ifr.ifr_addr = *(struct ::sockaddr*)&addr;
    ((struct ::sockaddr_in *)&ifr.ifr_addr)->sinaddr.s_addr = htonl(0xFFFFFF00);
    if ( -1 == ::ioctl(s, SIOCSIFADDR   , &ifr) ) goto End;
    if ( -1 == ::ioctl(s, SIOCSIFNETMASK, &ifr) ) goto End;
    if ( -1 == ::ioctl(s, SIOCGIFFLAGS, &ifr) ) goto End;
    ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
    if ( -1 == ::ioctl(s, SIOCSIFFLAGS, &ifr) ) goto End;
    //wipe_out_ipv6(fd,str_dev);
    retval = fd;
#else
    char buf[300u] = "ifconfig ";
    strcat(buf,str_dev);
    strcat(buf," ");
    strcat(buf,str_ip);
    strcat(buf," netmask 255.255.255.0");
    system(buf);
#endif
}
