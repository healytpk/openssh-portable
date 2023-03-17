#ifndef HPP__MAIN
#define HPP__MAIN

#include <wx/app.h>             // wxApp

class App_gui_openssh_vpn : public wxApp {
public:

	void RecreateGUI(void);

    bool OnInit(void) override;  /* Defined in main.cpp */

	int OnExit(void) override { return 0; }
};

DECLARE_APP(App_gui_openssh_vpn);

#endif //ifndef HPP__MAIN
