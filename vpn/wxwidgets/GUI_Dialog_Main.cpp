#include "GUI_Dialog_Main.hpp"

#include <cassert>  // assert
#include <cstddef>  // size_t
#include <cctype>   // isprint, isspace
#include <string>   // string
#include <thread>   // jthread
#include <chrono>   // milliseconds
#include <string_view>  // string_view

#include <wx/app.h>     // wxApp
#include <wx/msgdlg.h>  // wxMessageBox
#include "wx_custom_event.hpp"  // EventClass_StringView

#include "../ostream_redirect.hpp"  // RedirectAllOutput, ReportGUIFinished
#include "../vpn-core.h"            // g_VPN_excluded_routes

DEFINE_EVENT_TYPE( g_event_type_string_view )

extern "C" int ssh_client_begin(int,char**);         // define in ssh.c
extern "C" void Set_Password_From_GUI(char const*);  // defined in readpass.c

using std::string;

Dialog_Main *g_p_dlgmain = nullptr;

Dialog_Main::Dialog_Main( wxWindow* parent )
  : Dialog_Main__Auto_Base_Class( parent )
{
    this->Connect( g_event_type_string_view, EventHandler_StringView(Dialog_Main::OnReceiveText) );
}

// I've made sure the function on the next line returns uint32_t and not
// unsigned long. The value returned is in NetworkByte order (i.e. BigEndian)
extern "C" std::uint32_t inet_addr(char const*);

void Dialog_Main::OnButtonClick_Connect(wxCommandEvent&)
{
    static std::jthread j;

    static char *argv[] = {
	"ssh",
	"--vpn",
	"-D", "5555",
	nullptr,
	"/home/dh_p7rcrw/progs/badvpn-udpgw", "--listen-addr", "127.0.0.1:7300",
	nullptr,
	nullptr,
	nullptr,
    };

    static char buf[255u];
    strcpy(buf, this->m_textCtrl_Username->GetValue().ToAscii());
    strcat(buf,"@");
    strcat(buf, this->m_textCtrl_RemoteServer->GetValue().ToAscii());
    argv[4] = buf;

    string const password( this->m_textCtrl_Password->GetValue().mb_str() );

    if ( password.empty() )
    {
	wxMessageBox("Password cannot be blank", "Error", wxICON_ERROR|wxCENTRE|wxOK, this);
	return;
    }

    Set_Password_From_GUI( password.c_str() );

    g_VPN_excluded_routes.clear();

    for ( unsigned i = 0u; i < (sizeof(this->mRoute_checkBox) / sizeof*(this->mRoute_checkBox)); ++i )
    {
	if ( false == this->mRoute_checkBox[i]->GetValue() ) continue;

	uint32_t const addr = ::inet_addr(this->mRoute_Text_Net    [i]->GetValue().ToAscii()),
	               mask = ::inet_addr(this->mRoute_Text_Netmask[i]->GetValue().ToAscii());

	g_VPN_excluded_routes.emplace_back(addr,mask);
    }

    RedirectAllOutput();

    j = std::jthread(
	[&argv](std::stop_token)->void
	  {
	      fputs("A: I like my seven axolotls\n", stdout);
	      std::this_thread::sleep_for( std::chrono::milliseconds(250u) );
	      puts("B: I like my seven axolotls");
	      std::this_thread::sleep_for( std::chrono::milliseconds(250u) );
	      fprintf(stdout, "C: I like my seven axolotls\n");
	      std::this_thread::sleep_for( std::chrono::milliseconds(250u) );
	      printf("D: I like my seven axolotls\n");
	      std::this_thread::sleep_for( std::chrono::milliseconds(250u) );
	      fprintf(stderr, "E: ERR - I like my seven axolotls\n");
	      std::this_thread::sleep_for( std::chrono::milliseconds(250u) );
	      std::cerr << "F: ERR - I like my seven axolotls\n" << std::flush;
	      std::this_thread::sleep_for( std::chrono::milliseconds(250u) );
	      std::clog << "G: ERR - I like my seven axolotls\n";
	      std::this_thread::sleep_for( std::chrono::milliseconds(250u) );
	      ssh_client_begin(8, argv);
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

	if ( ('\0' == c) || (false == (isprint(c) || isspace(c))) )
	{
	    s.erase(i,1u);
	    --i;
	}
    }

    this->m_textTerminal->AppendText(s.c_str());

    ReportGUIFinished();
}

void Dialog_Main::OnClose( wxCloseEvent& event )
{
    this->Destroy();
}

static std::string str;

extern "C" ssize_t wxwidgets_writer(void*, char const *const buffer, size_t const size)
{
#warning Move binary_semaphore to this file and this function
    str = std::string(buffer,size);

    // Newer versions of wxWidgets allow the following:
    //   g_p_dlgmain->CallAfter(&Dialog_Main::CallAfter_Receive_Text, std::string_view(buffer,size) );
    // but for older versions we must use a custom event type

    EventClass_StringView event(g_event_type_string_view,0);

    event.sv = std::string_view(str);

    assert( nullptr != g_p_dlgmain );

    g_p_dlgmain->GetEventHandler()->AddPendingEvent(event);

    return size;
}
