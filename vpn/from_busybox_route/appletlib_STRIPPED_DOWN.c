/* vi: set sw=4 ts=4: */
/*
 * Utility routines.
 *
 * Copyright (C) tons of folks.  Tracking down who wrote what
 * isn't something I'm going to worry about...  If you wrote something
 * here, please feel free to acknowledge your work.
 *
 * Based in part on code from sash, Copyright (c) 1999 by David I. Bell
 * Permission has been granted to redistribute this code under GPL.
 *
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */
/* We are trying to not use printf, this benefits the case when selected
 * applets are really simple. Example:
 *
 * $ ./busybox
 * ...
 * Currently defined functions:
 *         basename, false, true
 *
 * $ size busybox
 *    text    data     bss     dec     hex filename
 *    4473      52      72    4597    11f5 busybox
 *
 * FEATURE_INSTALLER or FEATURE_SUID will still link printf routines in. :(
 */

/* Define this accessor before we #define "errno" our way */
#include <errno.h>
static inline int *get_perrno(void) { return &errno; }

#include "busybox.h"

#if !(defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) \
    || defined(__APPLE__) \
    )
# include <malloc.h> /* for mallopt */
#endif

/* Declare <applet>_main() */
#define PROTOTYPES
#include "applets.h"
#undef PROTOTYPES

/* Include generated applet names, pointers to <applet>_main, etc */
#include "applet_tables.h"
/* ...and if applet_tables generator says we have only one applet... */
#ifdef SINGLE_APPLET_MAIN
# undef ENABLE_FEATURE_INDIVIDUAL
# define ENABLE_FEATURE_INDIVIDUAL 1
# undef IF_FEATURE_INDIVIDUAL
# define IF_FEATURE_INDIVIDUAL(...) __VA_ARGS__
#endif

#include "usage_compressed.h"

#if ENABLE_FEATURE_SH_EMBEDDED_SCRIPTS
# define DEFINE_SCRIPT_DATA 1
# include "embedded_scripts.h"
#else
# define NUM_SCRIPTS 0
#endif
#if NUM_SCRIPTS > 0
# include "bb_archive.h"
static const char packed_scripts[] ALIGN1 = { PACKED_SCRIPTS };
#endif

/* "Do not compress usage text if uncompressed text is small
 *  and we don't include bunzip2 code for other reasons"
 *
 * Useful for mass one-applet rebuild (bunzip2 code is ~2.7k).
 *
 * Unlike BUNZIP2, if FEATURE_SEAMLESS_BZ2 is on, bunzip2 code is built but
 * still may be unused if none of the selected applets calls open_zipped()
 * or its friends; we test for (FEATURE_SEAMLESS_BZ2 && <APPLET>) instead.
 * For example, only if TAR and FEATURE_SEAMLESS_BZ2 are both selected,
 * then bunzip2 code will be linked in anyway, and disabling help compression
 * would be not optimal:
 */
#if UNPACKED_USAGE_LENGTH < 4*1024 \
 && !(ENABLE_FEATURE_SEAMLESS_BZ2 && ENABLE_TAR) \
 && !(ENABLE_FEATURE_SEAMLESS_BZ2 && ENABLE_MODPROBE) \
 && !(ENABLE_FEATURE_SEAMLESS_BZ2 && ENABLE_INSMOD) \
 && !(ENABLE_FEATURE_SEAMLESS_BZ2 && ENABLE_DEPMOD) \
 && !(ENABLE_FEATURE_SEAMLESS_BZ2 && ENABLE_MAN) \
 && !ENABLE_BUNZIP2 \
 && !ENABLE_BZCAT
# undef  ENABLE_FEATURE_COMPRESS_USAGE
# define ENABLE_FEATURE_COMPRESS_USAGE 0
#endif


unsigned FAST_FUNC string_array_len(char **argv)
{
	char **start = argv;

	while (*argv)
		argv++;

	return argv - start;
}


#if ENABLE_SHOW_USAGE && !ENABLE_FEATURE_COMPRESS_USAGE
static const char usage_messages[] ALIGN1 = UNPACKED_USAGE;
#else
# define usage_messages 0
#endif

#if ENABLE_FEATURE_COMPRESS_USAGE

static const char packed_usage[] ALIGN1 = { PACKED_USAGE };
# include "bb_archive.h"
# define unpack_usage_messages() \
	unpack_bz2_data(packed_usage, sizeof(packed_usage), sizeof(UNPACKED_USAGE))
# define dealloc_usage_messages(s) free(s)

#else

# define unpack_usage_messages() usage_messages
# define dealloc_usage_messages(s) ((void)(s))

#endif /* FEATURE_COMPRESS_USAGE */


void FAST_FUNC bb_show_usage(void)
{
    /* Code removed by TPK Healy on 28 Jan 2023 */
    for (;;);  // This function shouldn't return
}

int FAST_FUNC find_applet_by_name(const char *name)
{
	unsigned i;
	int j;
	const char *p;

/* The commented-out word-at-a-time code is ~40% faster, but +160 bytes.
 * "Faster" here saves ~0.5 microsecond of real time - not worth it.
 */
#if 0 /*BB_UNALIGNED_MEMACCESS_OK && BB_LITTLE_ENDIAN*/
	uint32_t n32;

	/* Handle all names < 2 chars long early */
	if (name[0] == '\0')
		return -1; /* "" is not a valid applet name */
	if (name[1] == '\0') {
		if (!ENABLE_TEST)
			return -1; /* 1-char name is not valid */
		if (name[0] != ']')
			return -1; /* 1-char name which isn't "[" is not valid */
		/* applet "[" is always applet #0: */
		return 0;
	}
#endif

	p = applet_names;
#if KNOWN_APPNAME_OFFSETS <= 0
	i = 0;
#else
	i = NUM_APPLETS * (KNOWN_APPNAME_OFFSETS - 1);
	for (j = ARRAY_SIZE(applet_nameofs)-1; j >= 0; j--) {
		const char *pp = applet_names + applet_nameofs[j];
		if (strcmp(name, pp) >= 0) {
			//bb_error_msg("name:'%s' >= pp:'%s'", name, pp);
			p = pp;
			break;
		}
		i -= NUM_APPLETS;
	}
	i /= (unsigned)KNOWN_APPNAME_OFFSETS;
	//bb_error_msg("name:'%s' starting from:'%s' i:%u", name, p, i);
#endif

	/* Open-coded linear search without strcmp/strlen calls for speed */
	while (*p) {
		/* Do we see "name\0" at current position in applet_names? */
		for (j = 0; *p == name[j]; ++j) {
			if (*p++ == '\0') {
				//bb_error_msg("found:'%s' i:%u", name, i);
				return i; /* yes */
			}
		}
		/* No. Have we gone too far, alphabetically? */
		if (*p > name[j]) {
			//bb_error_msg("break:'%s' i:%u", name, i);
			break;
		}
		/* No. Move to the start of the next applet name. */
		while (*p++ != '\0')
			continue;
		i++;
	}
	return -1;
}


void lbb_prepare(const char *applet
		IF_FEATURE_INDIVIDUAL(, char **argv))
				MAIN_EXTERNALLY_VISIBLE;
void lbb_prepare(const char *applet
		IF_FEATURE_INDIVIDUAL(, char **argv))
{
#ifdef bb_cached_errno_ptr
	ASSIGN_CONST_PTR(&bb_errno, get_perrno());
#endif
	applet_name = applet;

	if (ENABLE_LOCALE_SUPPORT)
		setlocale(LC_ALL, "");

#if ENABLE_FEATURE_INDIVIDUAL
	/* Redundant for busybox (run_applet_and_exit covers that case)
	 * but needed for "individual applet" mode */
	if (argv[1]
	 && strcmp(argv[1], "--help") == 0
	 && !is_prefixed_with(applet, "busybox")
	) {
		/* Special cases. POSIX says "test --help"
		 * should be no different from e.g. "test --foo".
		 */
		if (!(ENABLE_TEST && strcmp(applet_name, "test") == 0)
		 && !(ENABLE_TRUE && strcmp(applet_name, "true") == 0)
		 && !(ENABLE_FALSE && strcmp(applet_name, "false") == 0)
		 && !(ENABLE_ECHO && strcmp(applet_name, "echo") == 0)
		)
			bb_show_usage();
	}
#endif
}

/* The code below can well be in applets/applets.c, as it is used only
 * for busybox binary, not "individual" binaries.
 * However, keeping it here and linking it into libbusybox.so
 * (together with remaining tiny applets/applets.o)
 * makes it possible to avoid --whole-archive at link time.
 * This makes (shared busybox) + libbusybox smaller.
 * (--gc-sections would be even better....)
 */

const char *applet_name;
