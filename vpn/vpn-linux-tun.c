#ifdef __cplusplus
#error You must use a C compiler to compile this file (not a C++ compiler)
#include "Nonexistent_header_file_see_previous_error"
#endif

#include <stdio.h>           // sprintf
#include <stdlib.h>          // system
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

#define nullptr (0)

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
    system("sudo ifconfig tun0 up");

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

int tun_alloc(void)
{
    struct ifreq ifr = {0};
    int fd, err;

    if ( (fd = open("/dev/net/tun", O_RDWR)) < 0 ) return fd;

    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

    if ( (err = ioctl(fd, TUNSETIFF, &ifr)) < 0 )
    {
        close(fd);
        return err;
    }

/*
    if ( -1 == setip(fd, ifr.ifr_name, "10.10.10.1") )
    {
        close(fd);
        fd = -1;
    }
*/

    return fd;
}
