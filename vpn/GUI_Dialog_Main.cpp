#include "GUI_Dialog_Main.hpp"

#include <cassert>  // assert
#include <cstddef>  // size_t
#include <cctype>   // isprint
#include <cstring>  // strcmp, strstr
#include <string>   // string
#include <vector>   // vector
#include <filesystem>  // directory_iterator
#include <thread>      // jthread
#include <chrono>      // milliseconds

#include <wx/app.h>     // wxApp
#include <wx/msgdlg.h>  // wxMessageBox

#include "ostream_redirect.hpp"  // streambuf_redirect

using std::string;

Dialog_Main *g_p_dlgmain = nullptr;

extern "C" int ssh_client_main(int,char**);  // defined in ssh.c

Dialog_Main::Dialog_Main( wxWindow* parent )
  : Dialog_Main__Auto_Base_Class( parent )
{
    this->Connect( g_event_type_string_view, EventHandler_StringView(Dialog_Main::OnReceiveText) );
    /* Nothing to do in here */
}

extern "C" void Set_Password_From_GUI(char const*);  // defined in readpass.c

void Dialog_Main::OnButtonClick_Connect(wxCommandEvent&)
{
    static std::jthread j;

    static char *argv[] = {
	"ssh",
	"--vpn",
	"-D", "5555",
	"dh_p7rcrw@virjacode.com",
	"/home/dh_p7rcrw/progs/badvpn-udpgw", "--listen-addr", "127.0.0.1:7300",
	nullptr,
	nullptr,
	nullptr,
    };

    std::string password( this->m_textCtrl_Password->GetValue().mb_str() );

    if ( password.empty() )
    {
	wxMessageBox("Password cannot be blank", "Error", wxICON_ERROR|wxCENTRE|wxOK, this);
	return;
    }

    Set_Password_From_GUI( password.c_str() );

    //wxMessageBox("I wish I was a frog","Monkey Fish",0,0);

    //Redirect_Output::printf("Hello my name is Gregory\n");
    //Redirect_Output::puts("Hello my name is Gregory");

    j = std::jthread(
	[&argv](std::stop_token)->void
	  {
	      fputs("A: I like my seven axolotls\n", Redirect_Output::GetHandle());
	      std::this_thread::sleep_for( std::chrono::milliseconds(250u) );
	      Redirect_Output::RedirectAll();
	      fputs("B: I like my seven axolotls\n", stdout);
	      std::this_thread::sleep_for( std::chrono::milliseconds(250u) );
	      puts("C: I like my seven axolotls");
	      std::this_thread::sleep_for( std::chrono::milliseconds(250u) );
	      fprintf(stdout, "D: I like my seven axolotls\n");
	      std::this_thread::sleep_for( std::chrono::milliseconds(250u) );
	      printf("E: I like my seven axolotls\n");
	      std::this_thread::sleep_for( std::chrono::milliseconds(250u) );
	      fprintf(stderr, "F: ERR - I like my seven axolotls\n");
	      std::this_thread::sleep_for( std::chrono::milliseconds(250u) );
	      std::cerr << "G: ERR - I like my seven axolotls\n" << std::flush;
	      std::this_thread::sleep_for( std::chrono::milliseconds(250u) );
	      std::clog << "H: ERR - I like my seven axolotls\n";
	      std::this_thread::sleep_for( std::chrono::milliseconds(250u) );
	      //fflush(Redirect_Output::GetHandle());
              ssh_client_main(8, argv);
	  });
}

void Dialog_Main::OnReceiveText(EventClass_StringView &ecs)
{
    assert( wxIsMainThread() );

    string s( ecs.sv );

    // The following loop removes any char that isn't printable

    for ( std::size_t i = 0u; i < s.size(); ++i )
    {
	using std::isprint;
	using std::isspace;

	char unsigned c = s[i];

	if ( false == (isprint(c) || isspace(c)) )
	{
	    s.erase(i,1u);
	    --i;
	}
    }

    this->m_textTerminal->AppendText(s.c_str());
}

void Dialog_Main::OnClose( wxCloseEvent& event )
{
    this->Destroy();
}
