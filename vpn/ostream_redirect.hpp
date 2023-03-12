#ifndef HEADER_INCLUSION_GUARDS_OSTREAM_REDIRECT
#define HEADER_INCLUSION_GUARDS_OSTREAM_REDIRECT

#ifdef __cplusplus

#include <cstddef>
#include <cstdio>
#include <streambuf>  // streambuf

namespace Redirect_Output {

    ssize_t reader(void *cookie, char *buffer, size_t size);

    extern "C" ssize_t writer(void *cookie, const char *buffer, size_t size);

    int seeker(void *cookie, off64_t *position, int whence);

    int cleaner(void *cookie);

    ::FILE *GetHandle(void);

    int printf(char const *const fmt, ... );

    int puts(char const *const s);

    void RedirectAll(void);
}

#else

ssize_t writer(void *cookie, const char *buffer, size_t size);

#endif  // ifdef __cplusplus

#endif
