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
    wxTextCtrl *const mRoute_Text_Net    [4u] = { mRoute_Text_Net_00    , mRoute_Text_Net_01    , mRoute_Text_Net_02    , mRoute_Text_Net_03     };
    wxTextCtrl *const mRoute_Text_Netmask[4u] = { mRoute_Text_Netmask_00, mRoute_Text_Netmask_01, mRoute_Text_Netmask_02, mRoute_Text_Netmask_03 };
    wxCheckBox *const mRoute_checkBox    [4u] = { mRoute_checkBox_00    , mRoute_checkBox_01    , mRoute_checkBox_02    , mRoute_checkBox_03     };
};

extern Dialog_Main *g_p_dlgmain;

#endif
