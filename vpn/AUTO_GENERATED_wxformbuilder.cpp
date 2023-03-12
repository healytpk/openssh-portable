///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "AUTO_GENERATED_wxformbuilder.h"

///////////////////////////////////////////////////////////////////////////

Dialog_Main__Auto_Base_Class::Dialog_Main__Auto_Base_Class( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_staticline3 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline3, 0, wxALL|wxEXPAND, 5 );

	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 3, 2, 0, 0 );

	m_staticText31 = new wxStaticText( this, wxID_ANY, wxT("Remote Server:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	gSizer1->Add( m_staticText31, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl_RemoteServer = new wxTextCtrl( this, wxID_ANY, wxT("virjacode.com"), wxDefaultPosition, wxSize( 200,-1 ), 0 );
	gSizer1->Add( m_textCtrl_RemoteServer, 0, wxALL|wxEXPAND, 5 );

	m_staticText41 = new wxStaticText( this, wxID_ANY, wxT("Username:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText41->Wrap( -1 );
	gSizer1->Add( m_staticText41, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl_Username = new wxTextCtrl( this, wxID_ANY, wxT("dh_p7rcrw"), wxDefaultPosition, wxSize( 200,-1 ), 0 );
	gSizer1->Add( m_textCtrl_Username, 0, wxALL|wxEXPAND, 5 );

	m_staticText311 = new wxStaticText( this, wxID_ANY, wxT("Password:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText311->Wrap( -1 );
	gSizer1->Add( m_staticText311, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl_Password = new wxTextCtrl( this, wxID_ANY, wxT("kErpuSkjq9"), wxDefaultPosition, wxSize( 200,-1 ), 0 );
	gSizer1->Add( m_textCtrl_Password, 0, wxALL|wxEXPAND, 5 );


	bSizer1->Add( gSizer1, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	m_buttonConnect = new wxButton( this, wxID_ANY, wxT("Connect"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonConnect, 0, wxALL, 5 );

	m_button4 = new wxButton( this, wxID_ANY, wxT("Advanced. . ."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_button4, 0, wxALL, 5 );


	bSizer1->Add( bSizer6, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	m_staticline31 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline31, 0, wxEXPAND | wxALL, 5 );

	wxGridSizer* gSizer11;
	gSizer11 = new wxGridSizer( 4, 2, 0, 0 );

	m_staticText312 = new wxStaticText( this, wxID_ANY, wxT("Name of Virtual Network Interface:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText312->Wrap( -1 );
	gSizer11->Add( m_staticText312, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl42 = new wxTextCtrl( this, wxID_ANY, wxT("tun0"), wxDefaultPosition, wxSize( 200,-1 ), 0 );
	gSizer11->Add( m_textCtrl42, 0, wxALL, 5 );

	m_staticText411 = new wxStaticText( this, wxID_ANY, wxT("IP Address of Virtual Network Interface:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText411->Wrap( -1 );
	gSizer11->Add( m_staticText411, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl51 = new wxTextCtrl( this, wxID_ANY, wxT("172.25.81.149"), wxDefaultPosition, wxSize( 200,-1 ), 0 );
	gSizer11->Add( m_textCtrl51, 0, wxALL, 5 );

	m_staticText4111 = new wxStaticText( this, wxID_ANY, wxT("VPN Netmask:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4111->Wrap( -1 );
	gSizer11->Add( m_staticText4111, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl4111 = new wxTextCtrl( this, wxID_ANY, wxT("255.255.255.252"), wxDefaultPosition, wxSize( 200,-1 ), 0 );
	gSizer11->Add( m_textCtrl4111, 0, wxALL, 5 );

	m_staticText3111 = new wxStaticText( this, wxID_ANY, wxT("VPN Default Gateway:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3111->Wrap( -1 );
	gSizer11->Add( m_staticText3111, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtrl411 = new wxTextCtrl( this, wxID_ANY, wxT("172.25.81.150"), wxDefaultPosition, wxSize( 200,-1 ), 0 );
	gSizer11->Add( m_textCtrl411, 0, wxALL, 5 );


	bSizer1->Add( gSizer11, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	m_staticline311 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline311, 0, wxEXPAND | wxALL, 5 );

	m_textTerminal = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	bSizer1->Add( m_textTerminal, 1, wxALL|wxEXPAND, 5 );

	m_button_ShowChangeLog = new wxButton( this, wxID_ANY, wxT("Show Change Log..."), wxDefaultPosition, wxSize( 150,35 ), 0 );
	bSizer1->Add( m_button_ShowChangeLog, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Dialog_Main__Auto_Base_Class::OnClose ) );
	m_buttonConnect->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::OnButtonClick_Connect ), NULL, this );
	m_button_ShowChangeLog->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::OnButtonClick_ShowChangeLog ), NULL, this );
}

Dialog_Main__Auto_Base_Class::~Dialog_Main__Auto_Base_Class()
{
}

Dialog_ChangeLog__Auto_Base_Class::Dialog_ChangeLog__Auto_Base_Class( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxVERTICAL );

	m_textCtrl19 = new wxTextCtrl( this, wxID_ANY, wxT("\nThis is the first version so there's no change log yet!"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	bSizer33->Add( m_textCtrl19, 1, wxALL|wxEXPAND, 5 );

	m_button_Close = new wxButton( this, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer33->Add( m_button_Close, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	this->SetSizer( bSizer33 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_button_Close->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_ChangeLog__Auto_Base_Class::OnButtonClick_Close ), NULL, this );
}

Dialog_ChangeLog__Auto_Base_Class::~Dialog_ChangeLog__Auto_Base_Class()
{
}
