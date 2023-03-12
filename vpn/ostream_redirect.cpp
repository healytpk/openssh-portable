#include "ostream_redirect.hpp"

#include <cassert>  // assert
#include <cstdio>   // vfprintf
#include <atomic>   // atomic_flag
#include <stdio.h>  // ssize_t, off64_t, cookie_iofunctions_t, fopencookie
#include <cstdarg>  // va_list, va_start, va_end
#include <cstring>  // strlen
#include <string>   // string

//#include <cxxabi.h>
#include <ext/stdio_filebuf.h>

#include "GUI_Dialog_Main.hpp"  // g_p_dlgmain
#include "wx_custom_event.hpp"  // EventClass_StringView

#include <fstream>     // REMOVE THIS
#include <iostream>    // endl
#include <streambuf>

namespace Redirect_Output {

    std::atomic_flag g_prevent_reentry = ATOMIC_FLAG_INIT;

    ssize_t reader(void *cookie, char *buffer, size_t size)
    {
        return size;
    }

    extern "C" ssize_t writer(void *cookie, const char *buffer, size_t size)
    {
        if ( g_prevent_reentry.test_and_set() ) return 0;

        static std::string str;

        str = std::string(buffer,size);

        //assert( false == wxIsMainThread() );

        //g_p_dlgmain->CallAfter( &Dialog_Main::CallAfter_Receive_Text, std::string_view(buffer,size) );

        EventClass_StringView event(g_event_type_string_view,0);

        event.sv = std::string_view(str);

        g_p_dlgmain->GetEventHandler()->AddPendingEvent(event);

        g_prevent_reentry.clear();  // REVISIT - FIX - Use a scope guard for this in case an exception is thrown

        return size;
    }

    int seeker(void *cookie, off64_t *position, int whence)
    {
        return 0;
    }

    int cleaner(void *cookie)
    {
        return 0;
    }

    ::FILE *GetHandle(void)
    {
        static cookie_io_functions_t g_funcptrs = {};  // all null

        g_funcptrs.write = writer;

        static ::FILE *const fp = ::fopencookie(nullptr, "w", g_funcptrs);

        assert( nullptr != fp );

        std::setvbuf(fp, nullptr, _IOLBF, 0);  // line buffered

        return fp;
    }

    int printf(char const *const fmt, ... )
    {
        va_list args;
        va_start(args, fmt);
        std::vfprintf( GetHandle(), fmt, args );
        std::fflush( GetHandle() );
        va_end(args);
        return 0;
    }

    int puts(char const *const s)
    {
        fputs( s, GetHandle() );
        std::fflush( GetHandle() );
        return 0;
    }

    class streambuf_redirect : public std::streambuf {

        using std::streambuf::streambuf;

        virtual std::streamsize xsputn(char const *const s, std::streamsize const count)
        {
            return writer(nullptr,s,count);
        }
    };

    void RedirectAll(void)
    {
        stdout = stderr = GetHandle();

        //static __gnu_cxx::stdio_filebuf<char> buf0( GetHandle(), std::ios::out );
        //static __gnu_cxx::stdio_filebuf<char> buf1( GetHandle(), std::ios::out );
        //static __gnu_cxx::stdio_filebuf<char> buf2( GetHandle(), std::ios::out );

        static streambuf_redirect buf0,buf1,buf2;

        std::cout.rdbuf( &buf0 );
        std::clog.rdbuf( &buf1 );
        std::cerr.rdbuf( &buf2 );
    }
}
