AM_CFLAGS = -std=c99
AM_CPPFLAGS = -Wall -DDEBUG -D_GNU_SOURCE
AM_LDFLAGS = -static

bin_PROGRAMS =
lib_LTLIBRARIES =
BUILT_SOURCES =
CLEANFILES = $(BUILT_SOURCES)

lib_LTLIBRARIES += libmddl.la
libmddl_la_SOURCES = mddl.c mddl.h
libmddl_la_CPPFLAGS = $(AM_CPPFLAGS) $(LIBXML_CFLAGS)

bin_PROGRAMS += mddl-code
mddl_code_CPPFLAGS = $(AM_CPPFLAGS) $(LIBXML_CFLAGS)
mddl_code_LDFLAGS = $(LIBXML_LIBS)
BUILT_SOURCES += mddl-code-opt.c

## our rule for gengetopt
%.c: %.ggo
	gengetopt -l -i $< -F $*

## Help the developers get nice post-processed source files

## Create preprocessor output (debugging purposes only)
%.i: %.c
	$(COMPILE) -E -o $@ $<

## Create assembler output (debugging purposes only)
%.s: %.c
	$(COMPILE) -S -c $(AM_CFLAGS) $<

## Makefile.am ends here
