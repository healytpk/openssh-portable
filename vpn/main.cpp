#include "main.hpp"

#include <cstdlib>  // EXIT_FAILURE, exit, abort
#include <cstdint>  // uintptr_t, uint64_t
#include <cstdio>   // puts
#include <iostream> // cerr, endl

#include <dlfcn.h>  // dlopen

#include "wx_custom_event.hpp"   // EventClass_StringView
#include "GUI_Dialog_Main.hpp"   // Dialog_Main

using std::cout; using std::cerr; using std::endl;

#if 0

DECLARE_APP(App_gui_openssh_vpn);
IMPLEMENT_APP(App_gui_openssh_vpn);

#elif 0

extern App_gui_openssh_vpn &wxGetApp(void);

App_gui_openssh_vpn &wxGetApp(void)
{
    return *static_cast<App_gui_openssh_vpn*>(wxApp::GetInstance());
}

wxAppConsole *wxCreateApp(void)
{
    wxAppConsole::CheckBuildOptions(
        "3" "." "2" " (" "wchar_t" ",compiler with C++ ABI compatible with gcc 4" ",wx containers" ",compatible with 3.0" ")", "your program");

    return new App_gui_openssh_vpn;
}

wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp);

#else

extern App_gui_openssh_vpn& wxGetApp(void);

wxAppConsole *wxCreateApp()
{
    wxAppConsole::CheckBuildOptions(
        "2" "." "8" " (" "debug" "," "ANSI" ",compiler with C++ ABI " "1016" ",wx containers" ",compatible with 2.6" ")", "your program");
        return new App_gui_openssh_vpn;
}

wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp);

extern App_gui_openssh_vpn& wxGetApp();
App_gui_openssh_vpn& wxGetApp()
{
    return *static_cast<App_gui_openssh_vpn*>(wxApp::GetInstance());
}

/*
int main(int argc, char **argv)
{
    return wxEntry(argc, argv);
}
*/

extern bool wxThemeUsewin32;
static struct wxThemeUserForwin32 { wxThemeUserForwin32() { wxThemeUsewin32 = true; } } wxThemeDoUsewin32;;

#endif

void App_gui_openssh_vpn::RecreateGUI(void)
{
    wxWindow *const topwindow = GetTopWindow();

    if ( topwindow )
    {
        SetTopWindow(NULL);
        topwindow->Destroy();
    }

    g_p_dlgmain = new Dialog_Main(nullptr);

    g_p_dlgmain->Show();   /* Just let this throw if it fails */
}

bool App_gui_openssh_vpn::OnInit(void)
{
	wxThread::SetConcurrency(1u);

	this->RecreateGUI();

	return true;
}

extern "C" void Load_GUI_Libraries(void);

extern "C" int ssh_client_main(int,char**);

//char *__progname = "ssh";

extern "C" void dummy_func_from_dummy_library(void);

extern "C" void load_libs(void)  // gets called from pre_start (before _start)
{
    //::dlopen("lib9.so7",RTLD_LAZY|RTLD_GLOBAL);
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
        if ( argc < 2 )
        {
            Load_GUI_Libraries();
            return wxEntry(argc, argv);
        }
    }
    catch(...)
    {
        std::abort();
    }

    cout << "This is the console program :-)" << endl;
    return ssh_client_main(argc,argv);
}
