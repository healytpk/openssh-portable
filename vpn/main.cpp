#define _GNU_SOURCE
#include <cassert>  // assert
#include <iostream> // cerr, endl
#include <dlfcn.h>  // dlopen

struct ProgramModality {
    enum class ProgramType : unsigned {
        undefined=0,
        console=1,
        gtk3,
        x11
    } mode;

    int (*funcptr_main)(int,char**);

} g_program_modality = { ProgramModality::ProgramType::undefined, nullptr };

extern     int main_gtk3      (int,char**);
extern     int main_x11       (int,char**);
extern "C" int main_ssh_client(int,char**);

#if 1
        static char const *const g_strs_dyn_libs[] = {
            "libgtk-3.so",
            //"libX11.so",
            nullptr
        };
#elif 0  // These are the one's from readelf -a | grep NEEDED
        static char const *const g_strs_dyn_libs[] = {
            "libpng16.so.16",
            "libfontconfig.so.1",
            "libglib-2.0.so.0",
            "libgobject-2.0.so.0",
            "libpango-1.0.so.0",
            "libpangoft2-1.0.so.0",
            "libgio-2.0.so.0",
            "libgdk_pixbuf-2.0.so.0",
            "libcairo.so.2",
            "libpangocairo-1.0.so.0",
            "libgdk-3.so.0",
            "libgtk-3.so.0",
            "libSM.so.6",
            "libX11.so.6",
            nullptr
        };
#else  // These are the one's from ldd
        static char const *const g_strs_dyn_libs[] = {
            "libcrypto.so.3",
            "libz.so.1",
            "libX11.so.6",
            "libSM.so.6",
            "libgtk-3.so.0",
            "libgdk-3.so.0",
            "libpangocairo-1.0.so.0",
            "libcairo.so.2",
            "libgdk_pixbuf-2.0.so.0",
            "libgio-2.0.so.0",
            "libpangoft2-1.0.so.0",
            "libpango-1.0.so.0",
            "libgobject-2.0.so.0",
            "libglib-2.0.so.0",
            "libfontconfig.so.1",
            "libpng16.so.16",
            "libstdc++.so.6",
            "libm.so.6",
            "libgcc_s.so.1",
            "libc.so.6",
            "libxcb.so.1",
            "libICE.so.6",
            "libuuid.so.1",
            "libgmodule-2.0.so.0",
            "libXi.so.6",
            "libXfixes.so.3",
            "libcairo-gobject.so.2",
            "libatk-1.0.so.0",
            "libatk-bridge-2.0.so.0",
            "libepoxy.so.0",
            "libfribidi.so.0",
            "libharfbuzz.so.0",
            "libXinerama.so.1",
            "libXrandr.so.2",
            "libXcursor.so.1",
            "libXcomposite.so.1",
            "libXdamage.so.1",
            "libxkbcommon.so.0",
            "libwayland-cursor.so.0",
            "libwayland-egl.so.1",
            "libwayland-client.so.0",
            "libXext.so.6",
            "libpixman-1.so.0",
            "libfreetype.so.6",
            "libxcb-shm.so.0",
            "libxcb-render.so.0",
            "libXrender.so.1",
            "libjpeg.so.8",
            "libmount.so.1",
            "libselinux.so.1",
            "libthai.so.0",
            "libffi.so.8",
            "libpcre.so.3",
            "libexpat.so.1",
            "libXau.so.6",
            "libXdmcp.so.6",
            "libbsd.so.0",
            "libdbus-1.so.3",
            "libatspi.so.0",
            "libgraphite2.so.3",
            "libbrotlidec.so.1",
            "libblkid.so.1",
            "libpcre2-8.so.0",
            "libdatrie.so.1",
            "libmd.so.0",
            "libsystemd.so.0",
            "libbrotlicommon.so.1",
            "liblzma.so.5",
            "libzstd.so.1",
            "liblz4.so.1",
            "libcap.so.2",
            "libgcrypt.so.20",
            "libgpg-error.so.0",
            nullptr,
    };

#endif

bool Load_GUI_Libraries_GTK3(void)
{
    using std::cerr; using std::endl;

    cerr << "======= Libraries loaded with dlopen: ";

    for ( char const *const *pp = g_strs_dyn_libs; nullptr != *pp; ++pp )
    {
        if ( nullptr == dlopen(*pp, RTLD_LAZY | RTLD_GLOBAL) )
        {
            cerr << "ERROR loading library: " << *pp << endl;
            return false;
        }

        cerr << *pp << " " << std::flush;
    }

    cerr << endl;

    return true;
}

bool Load_GUI_Libraries_X11(void)
{
    return false;
}

int main(int argc, char **argv)
{
    // Before main has been entered, we had:
    //  pre_start -> _start -> _libc_start_main -> main

/*
    static char const str_libc[] = "lib9.so";

    if ( nullptr == dlopen(str_libc, 0x101) )
    {
        cerr << "ERROR loading library: " << str_libc << endl;
        std::abort();
    }
*/

    //dummy_func_from_dummy_library();

    try
    {
        if ( (argc < 2) && Load_GUI_Libraries_GTK3() )
        {
            g_program_modality = ProgramModality{ ProgramModality::ProgramType::gtk3,    main_gtk3       };
        }
        else if ( (argc < 2) && Load_GUI_Libraries_X11() )
        {
            g_program_modality = ProgramModality{ ProgramModality::ProgramType::x11,     main_x11        };
        }
        else
        {
            g_program_modality = ProgramModality{ ProgramModality::ProgramType::console, main_ssh_client };
        }

        assert( nullptr != g_program_modality.funcptr_main );
        return g_program_modality.funcptr_main(argc,argv);
    }
    catch(...)
    {
        // just fall through
    }

    return EXIT_FAILURE;
}
