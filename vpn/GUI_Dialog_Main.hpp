#ifndef H__dynamoMain
#define H__dynamoMain

#include "AUTO_GENERATED_wxformbuilder.h"

class Dialog_Main : public Dialog_Main__Auto_Base_Class {

protected:

    void OnClose( wxCloseEvent &event ) override;

public:

	Dialog_Main( wxWindow *parent );
};

extern Dialog_Main *g_p_dlgmain;

#endif
