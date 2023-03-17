#ifndef H__dynamoMain
#define H__dynamoMain

#include "AUTO_GENERATED_wxformbuilder.h"
#include "wx_custom_event.hpp"   // EventClass_StringView

class Dialog_Main : public Dialog_Main__Auto_Base_Class {

protected:

    void OnClose( wxCloseEvent &event ) override;

    void OnButtonClick_Connect(wxCommandEvent&) override;

    void OnReceiveText(EventClass_StringView&);

public:

	Dialog_Main( wxWindow *parent );
};

extern Dialog_Main *g_p_dlgmain;

#endif
