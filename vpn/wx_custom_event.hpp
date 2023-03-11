#ifndef HPP_STRING_VIEW_CUSTOM_WX_EVENT
#define HPP_STRING_VIEW_CUSTOM_WX_EVENT

#include <string_view>  // string_view

#include <wx/wx.h>      // wxEvent, wxEventType

DECLARE_EVENT_TYPE( g_event_type_string_view, -1 )  /* This line *declares* a global object */

struct EventClass_StringView : wxCommandEvent {

    std::string_view sv;

    EventClass_StringView(wxEventType const arg_command_type = g_event_type_string_view, int const arg_id = 0)
      :  wxCommandEvent(arg_command_type, arg_id) {}

    /* No need for copy-constructor, the default supplied one will do */

    // Required for sending with wxPostEvent()
    wxEvent *Clone(void) const
    {
	return new EventClass_StringView(*this);
    }
};

typedef void (wxEvtHandler::*EventMemberFunctionPointer_StringView)(EventClass_StringView&);

// This #define simplifies the one below, and makes the syntax less
// ugly if you want to use Connect() instead of an event table.
#define EventHandler_StringView(func)                                         \
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)\
	wxStaticCastEvent(EventMemberFunctionPointer_StringView, &func)

// Define the event table entry. Yes, it really *does* end in a comma.
#define EVT_STRING_VIEW(id, fn)                                            \
	DECLARE_EVENT_TABLE_ENTRY( g_event_type_receive_string_view, id, wxID_ANY,  \
	(wxObjectEventFunction)(wxEventFunction)                     \
	(wxCommandEventFunction) wxStaticCastEvent(                  \
	EventMemberFunctionPointer_StringView, &fn ), (wxObject*) NULL ),

// Optionally, you can do a similar #define for EVT_STRING_VIEW_RANGE.
#define EVT_STRING_VIEW_RANGE(id1,id2, fn)                                 \
	DECLARE_EVENT_TABLE_ENTRY( g_event_type_receive_string_view, id1, id2,      \
	EventHandler_StringView(fn), (wxObject*) NULL ),

#endif
