#ifndef HEADER_INCLUSION_GUARDS_CLI_VPN_CORE
#define HEADER_INCLUSION_GUARDS_CLI_VPN_CORE

#ifdef __cplusplus

    namespace VPN {
        extern void VPN_Enable(void);
        extern bool ParseNetworks(int,int);
    }

#else

#    include <stdbool.h>  // bool, true, false
     extern void VPN_Enable(void);
     extern bool VPN_ParseNetworks(int,int);
     extern int  VPN_ThreadEntryPoint_ListenToTun(void*);

#endif

#endif
