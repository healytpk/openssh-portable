#ifdef __cplusplus
#error You must use a C compiler to compile this file (not a C++ compiler)
#include "Nonexistent_header_file_see_previous_error"
#endif

#define _GNU_SOURCE     /* Allows us to use RTLD_NEXT with dlsym */
#include <stdbool.h>         // bool, true, false
#include <stdio.h>           // sprintf
#include <stdlib.h>          // system, abort
#include <string.h>          // strncpy, memset, strcmp

#include <fcntl.h>           // open
#include <sys/socket.h>      // socket
#include <unistd.h>          // close
#include <sys/ioctl.h>       // ioctl
#include <linux/if.h>        // ifreq
#include <linux/if_tun.h>    // TUNSETIFF
#include <net/route.h>       // rtentry

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

void bring_interface_up(char const *const str_dev)
{
#if 0
    char buf[300u] = "ifconfig ";
    strcat(buf,str_dev);
    strcat(buf," up");
    system(buf);
#else
    struct ifreq ifr = {0};

    int const fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

    strncpy(ifr.ifr_name, str_dev, IFNAMSIZ);

    ifr.ifr_addr.sa_family = AF_INET;

    /* get */  ioctl(fd, SIOCGIFFLAGS, &ifr);
    ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
    /* set */  ioctl(fd, SIOCSIFFLAGS, &ifr);

    close(fd);
#endif
}

void set_ip(char const *const str_dev, char const *const str_ip, char const *const str_netmask)
{
#if 0
    char buf[300u] = "ifconfig ";
    strcat(buf,str_dev);
    strcat(buf," ");
    strcat(buf,str_ip);
    strcat(buf," netmask 255.255.255.0");
    system(buf);
#else
    struct ifreq ifr = {0};

    int const fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

    strncpy(ifr.ifr_name, str_dev, IFNAMSIZ);

    ifr.ifr_addr.sa_family = AF_INET;

    struct sockaddr_in *const addr = (struct sockaddr_in*)&ifr.ifr_addr;

    inet_pton(AF_INET, str_ip, &addr->sin_addr);
    ioctl(fd, SIOCSIFADDR, &ifr);

    inet_pton(AF_INET, str_netmask, &addr->sin_addr);
    ioctl(fd, SIOCSIFNETMASK, &ifr);

    close(fd);
#endif
}

int is_interface_up(char const *const str_interface)
{
    int const sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if ( sock < 0 ) return 0;

    int retval = 0;
    struct ifreq ifr = {0};
    strcpy(ifr.ifr_name, str_interface);
    if ( ioctl(sock, SIOCGIFFLAGS, &ifr) < 0 )
    {
        perror("SIOCGIFFLAGS");
        goto End;
    }

    retval = !!(ifr.ifr_flags & IFF_RUNNING);

End:
    close(sock);
    return retval;
}

void add_route(unsigned long const dest, unsigned long const netmask, unsigned long const gateway, char const *const str_dev, unsigned const metric)
{
    int const sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if ( sock < 0 ) return;

    struct rtentry route = {0};

    struct sockaddr_in *const addrGy = (struct sockaddr_in*)&route.rt_gateway;
    struct sockaddr_in *const addrDn = (struct sockaddr_in*)&route.rt_dst    ;
    struct sockaddr_in *const addrGk = (struct sockaddr_in*)&route.rt_genmask;

    addrGy->sin_family = addrDn->sin_family = addrGk->sin_family = AF_INET;
    addrGy->sin_addr.s_addr = gateway;
    addrDn->sin_addr.s_addr = dest   ;
    addrGk->sin_addr.s_addr = netmask;

    route.rt_flags = RTF_UP|RTF_GATEWAY;
    route.rt_metric = metric + 1u;  // This seems to be zero-based instead of one-based

    ioctl(sock, SIOCADDRT, &route);

    close(sock);
}
