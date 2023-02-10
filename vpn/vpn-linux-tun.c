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
	int (*ioctl)(int, unsigned long, ...);
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
