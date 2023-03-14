#include "ostream_redirect.hpp"

#define _GNU_SOURCE    // required for RTLD_NEXT with dlsym

#include <cassert>  // assert
#include <stdio.h>  // FILE, ssize_t, off64_t, cookie_iofunctions_t, fopencookie
#include <cstdarg>  // va_list, va_start, va_end
#include <cstring>  // strlen
#include <atomic>   // atomic<bool>
#include <string>   // string
#include <iostream> // cout, cerr, clog, endl
#include <streambuf>// streambuf
#include <semaphore>// binary_semaphore

#include <dlfcn.h>  // dlsym, RTLD_NEXT

//#include <cxxabi.h>
//#include <ext/stdio_filebuf.h>

#include "GUI_Dialog_Main.hpp"  // g_p_dlgmain
#include "wx_custom_event.hpp"  // EventClass_StringView

namespace {

    std::binary_semaphore lock(1);
    std::string str;

    std::atomic<bool> g_should_redirect{false};

    ssize_t writer(void*, char const *const buffer, size_t const size)
    {
        if ( (0u == size) || (nullptr == buffer) ) return 0;

        lock.acquire();

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

    class streambuf_redirect : public std::streambuf {

        using std::streambuf::streambuf;

        virtual std::streamsize xsputn(char const *const s, std::streamsize const count)
        {
            return writer(nullptr,s,count);
        }
    };
}

extern "C" ssize_t write(int const fd, void const *const buf, size_t const count)  // This takes the place of the function provided by glibc
{
    if ( g_should_redirect && ((1==fd) || (2==fd)) ) return writer(nullptr, static_cast<char const*>(buf), count);

    static ssize_t (*const p)(int,void const*,size_t) = reinterpret_cast<ssize_t(*)(int,void const*,size_t)>(  ::dlsym(RTLD_NEXT, "write")  );

    return p(fd,buf,count);  // 1 = stdout, 2 = stderr
}

void RedirectAllOutput(void)
{
    g_should_redirect = true;

    cookie_io_functions_t g_funcptrs = {};  // all nullptr

    g_funcptrs.write = writer;

    FILE *const fp = fopencookie(nullptr, "w", g_funcptrs);

    assert( nullptr != fp );

    setvbuf(fp, nullptr, _IOLBF, 0);  // line buffered

    stdout = stderr = fp;

    //static __gnu_cxx::stdio_filebuf<char> buf0( GetHandle(), std::ios::out );
    //static __gnu_cxx::stdio_filebuf<char> buf1( GetHandle(), std::ios::out );
    //static __gnu_cxx::stdio_filebuf<char> buf2( GetHandle(), std::ios::out );

    static streambuf_redirect buf;

    std::cout.rdbuf( &buf );
    std::clog.rdbuf( &buf );
    std::cerr.rdbuf( &buf );
}

void ReportGUIFinished(void)
{
    lock.release();
}
