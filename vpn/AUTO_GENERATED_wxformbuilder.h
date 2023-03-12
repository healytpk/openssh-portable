///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/statline.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class Dialog_Main__Auto_Base_Class
///////////////////////////////////////////////////////////////////////////////
class Dialog_Main__Auto_Base_Class : public wxDialog
{
	private:

	protected:
		wxStaticLine* m_staticline3;
		wxStaticText* m_staticText31;
		wxTextCtrl* m_textCtrl4;
		wxStaticText* m_staticText41;
		wxTextCtrl* m_textCtrl5;
		wxStaticText* m_staticText311;
		wxTextCtrl* m_textCtrl41;
		wxButton* m_buttonConnect;
		wxButton* m_button4;
		wxStaticLine* m_staticline31;
		wxStaticText* m_staticText312;
		wxTextCtrl* m_textCtrl42;
		wxStaticText* m_staticText411;
		wxTextCtrl* m_textCtrl51;
		wxStaticText* m_staticText4111;
		wxTextCtrl* m_textCtrl4111;
		wxStaticText* m_staticText3111;
		wxTextCtrl* m_textCtrl411;
		wxStaticLine* m_staticline311;
		wxTextCtrl* m_textTerminal;
		wxButton* m_button_ShowChangeLog;

		// Virtual event handlers, override them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnButtonClick_Connect( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonClick_ShowChangeLog( wxCommandEvent& event ) { event.Skip(); }


	public:

		Dialog_Main__Auto_Base_Class( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Connect to SSH Server as VPN"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 617,624 ), long style = wxCAPTION|wxCLOSE_BOX );

		~Dialog_Main__Auto_Base_Class();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Dialog_ChangeLog__Auto_Base_Class
///////////////////////////////////////////////////////////////////////////////
class Dialog_ChangeLog__Auto_Base_Class : public wxDialog
{
	private:

	protected:
		wxTextCtrl* m_textCtrl19;
		wxButton* m_button_Close;

		// Virtual event handlers, override them in your derived class
		virtual void OnButtonClick_Close( wxCommandEvent& event ) { event.Skip(); }


	public:

		Dialog_ChangeLog__Auto_Base_Class( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("LightBlue Firmware Updater - Change Log"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 428,334 ), long style = wxDEFAULT_DIALOG_STYLE );

		~Dialog_ChangeLog__Auto_Base_Class();

};

