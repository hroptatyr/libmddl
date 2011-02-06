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
mddl_code_SOURCES = mddl-code.c mddl-print.c
mddl_code_CPPFLAGS = $(AM_CPPFLAGS) $(LIBXML_CFLAGS)
mddl_code_LDADD = libmddl.la
mddl_code_LDFLAGS = $(AM_LDFLAGS) $(LIBXML_LIBS)
BUILT_SOURCES += mddl-code-opt.c mddl-tag.c mddl-attr.c

## our rule for gengetopt
%.c: %.ggo
	gengetopt -l -i $< -F $*

## our rule for gperf
%.c: %.gperf
	gperf $< --output-file $@

## Help the developers get nice post-processed source files

## Create preprocessor output (debugging purposes only)
%.i: %.c
	$(COMPILE) -E -o $@ $<

## Create assembler output (debugging purposes only)
%.s: %.c
	$(COMPILE) -S -c $(AM_CFLAGS) $<

## Makefile.am ends here
