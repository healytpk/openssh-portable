#include "main.hpp"

#include <cstdlib>  // EXIT_FAILURE, abort
#include <cstdint>  // uintptr_t
#include <cstdio>   // puts
#include <dlfcn.h>  // dlopen

/*
//extern wxAppConsole* (* wxAppConsoleBase::ms_appInitFn)(void) __attribute__((weak));

int main(int argc, char **argv)
{
    return wxEntry(argc, argv);
}

App_gui_openssh_vpn& wxGetApp() { return *static_cast<App_gui_openssh_vpn*>(wxApp::GetInstance()); }

wxAppConsole *wxCreateApp() { wxAppConsole::CheckBuildOptions("3" "." "1" "." "5" " (" "wchar_t" ",compiler with C++ ABI compatible with gcc 4" ",wx containers" ",compatible with 3.0" ")", "your program"); return new App_gui_openssh_vpn; }

wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp);;

int Load_GUI_Libraries(void) __attribute__((constructor));
int Load_GUI_Libraries(void)
{
    //std::abort();

    using std::uintptr_t;

    uintptr_t r = 1u;

    r *= (uintptr_t)dlopen("libwx_gtk3u_core-3.1.so.5", RTLD_NOW|RTLD_GLOBAL);
    r *= (uintptr_t)dlopen("libwx_baseu-3.1.so.5",      RTLD_NOW|RTLD_GLOBAL);

    if ( 0u == r )
    {
        std::puts("Cannot load shared libraries for wxWidgets");
        std::exit(EXIT_FAILURE);
    }

    return -1;
}

struct MyClass {
    MyClass(void)
    {
        Load_GUI_Libraries();
    }
};

//MyClass g_dummy;

int main2(int argc, char **argv)
{
    std::puts("========== First line of main ==========");
    wxEntryStart(argc,argv);
    std::puts("========== 2 line of main ==========");
    wxTheApp->CallOnInit();
    std::puts("========== 3 line of main ==========");
    wxTheApp->OnRun();
    std::puts("========== 4 line of main ==========");
    return 0;
}
*/

//IMPLEMENT_APP_NO_MAIN(App_gui_openssh_vpn); /* This creates the "main" function */
//IMPLEMENT_APP(App_gui_openssh_vpn); /* This creates the "main" function */

/* The method 'OnInit' is invoked from within the
    wxWidgets library, and cppcheck thinks that it's an
    unused function */
// cppcheck-suppress unusedFunction symbolName=OnInit

bool App_gui_openssh_vpn::OnInit(void)
{
	wxThread::SetConcurrency(1u);

	this->RecreateGUI();

	return true;
}
