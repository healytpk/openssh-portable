#ifndef HPP__MAIN
#define HPP__MAIN

#include <wx/app.h>             // wxApp
#include "GUI_Dialog_Main.hpp"  // Dialog_Main

class App_gui_openssh_vpn : public wxApp {
public:

	void RecreateGUI(void)
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

    bool OnInit(void) override;  /* Defined in main.cpp */

	int OnExit(void) override { return 0; }
};

DECLARE_APP(App_gui_openssh_vpn);

#endif //ifndef HPP__MAIN
