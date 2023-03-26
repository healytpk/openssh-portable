#ifndef HEADER_INCLUSION_GUARDS_MAIN
#define HEADER_INCLUSION_GUARDS_MAIN

#include <sys/types.h>  // ssize_t

struct ProgramModality {
    enum class ProgramType : unsigned {
        undefined=0,
        console=1,
        gtk3,
        x11
    } mode;

    int (*funcptr_main)(int,char**);
    ssize_t (*funcptr_wxwidgets_writer)(void*,char const*,size_t);
};

extern struct ProgramModality g_program_modality;

#endif
