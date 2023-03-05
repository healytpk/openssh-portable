#include "GUI_Dialog_Main.hpp"

#include <cassert>  // assert
#include <cstddef>  // size_t
#include <cstring>  // strcmp, strstr
#include <string>   // string
#include <vector>   // vector
#include <filesystem>  // directory_iterator

#include <wx/app.h>     // wxApp
#include <wx/msgdlg.h>  // wxMessageBox

Dialog_Main *g_p_dlgmain = nullptr;

Dialog_Main::Dialog_Main( wxWindow* parent )
  : Dialog_Main__Auto_Base_Class( parent )
{
    /* Nothing to do in here */
}

void Dialog_Main::OnClose( wxCloseEvent& event )
{
    this->Destroy();
}
