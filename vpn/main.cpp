#include "main.hpp"

#include <cstdlib>  // EXIT_FAILURE, exit
#include <cstdint>  // uintptr_t, uint64_t
#include <cstdio>   // puts
#include <iostream> // cerr, endl

#include <dlfcn.h>  // dlopen

#include "GUI_Dialog_Main.hpp"  // Dialog_Main

using std::cerr; using std::endl;

wxAppConsole *wxCreateApp(void)
{
    // REVISIT FIX - Must figure out the next line!
    //wxAppConsole::CheckBuildOptions("3" "." "1" "." "5" " (" "wchar_t" ",compiler with C++ ABI compatible with gcc 4" ",wx containers" ",compatible with 3.0" ")", "your program");
    return new App_gui_openssh_vpn;
}

wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction)wxCreateApp);

App_gui_openssh_vpn &wxGetApp(void)
{
    return *static_cast<App_gui_openssh_vpn*>(wxApp::GetInstance());
}

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

void Load_GUI_Libraries(void) __attribute__((constructor));
void Load_GUI_Libraries(void)
{
    cerr << "============== Load_GUI_Libraries - marked with __constructor__ ===============" << endl;

    bool ok = true;

    ok = ok && dlopen("libgtk-3.so.0", RTLD_NOW|RTLD_GLOBAL);
//  ok = ok && dlopen("", RTLD_NOW|RTLD_GLOBAL);
//  ok = ok && dlopen("",      RTLD_NOW|RTLD_GLOBAL);

    if ( ok ) return;


    cerr << "Cannot load shared libraries for GTK+3.0" << endl;
    std::exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    // Before main has been entered, we had:
    //  pre_start -> _start -> _libc_start_main -> main
    return wxEntry(argc, argv);
}
