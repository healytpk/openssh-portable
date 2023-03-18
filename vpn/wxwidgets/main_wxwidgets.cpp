#include "main_wxwidgets.hpp"

#include <cstdlib>  // EXIT_FAILURE, exit, abort
#include <cstdint>  // uintptr_t, uint64_t
#include <cstdio>   // puts
#include <iostream> // cerr, endl

#include "wx_custom_event.hpp"   // EventClass_StringView
#include "GUI_Dialog_Main.hpp"   // Dialog_Main

using std::cout; using std::cerr; using std::endl;

#if 0

DECLARE_APP(App_gui_openssh_vpn);
IMPLEMENT_APP(App_gui_openssh_vpn);

#else

wxAppConsole *wxCreateApp()
{
    wxAppConsole::CheckBuildOptions(
#ifdef __WXX11__
        "2" "." "8" " (" "debug" "," "ANSI" ",compiler with C++ ABI " "1016" ",wx containers" ",compatible with 2.6" ")", "your program");
#else
        "3" "." "2" " (" "wchar_t" ",compiler with C++ ABI compatible with gcc 4" ",wx containers" ",compatible with 3.0" ")", "your program");
#endif
        return new App_gui_openssh_vpn;
}

wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction)wxCreateApp);

App_gui_openssh_vpn &wxGetApp(void)
{
    return *static_cast<App_gui_openssh_vpn*>(wxApp::GetInstance());
}

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

//char *__progname = "ssh";

extern "C" void load_libs(void)  // gets called from pre_start (before _start)
{
    //::dlopen("lib9.so7",RTLD_LAZY|RTLD_GLOBAL);
}

extern "C" int begin(int argc, char **argv)  // This becomes wxX11_begin or wxGTK3_begin
{
    return wxEntry(argc,argv);
}

#ifdef __WXX11__
    extern bool wxThemeUsewin32;

    static struct wxThemeUserForwin32 {
        wxThemeUserForwin32(void)
        {
            wxThemeUsewin32 = true;
        }
    } wxThemeDoUsewin32;
#endif
