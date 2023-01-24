#ifndef HEADER_INCLUSION_GUARDS_CLI_VPN_CORE
#define HEADER_INCLUSION_GUARDS_CLI_VPN_CORE

#ifdef __cplusplus

    namespace VPN {
        extern void VPN_Enable(void);
        extern bool ParseNetworks(int,int);

        // The variable on the next line is defined in channels.c
        extern "C" void (*g_vpn_addr_of_func_to_notify_SOCKS_is_listening)(int fd);
    }

#else

#    include <stdbool.h>  // bool, true, false
     extern void VPN_Enable(void);
     extern bool VPN_ParseNetworks(int,int);
     extern int  VPN_ThreadEntryPoint_ListenToTun(void*);
     extern void VPN_Notify_SOCKS_Is_Listening(int fd);

     // The variable on the next line is defined in channels.c
     extern void (*g_vpn_addr_of_func_to_notify_SOCKS_is_listening)(int fd);

#endif

#endif
