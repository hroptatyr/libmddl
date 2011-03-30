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

bin_PROGRAMS += mddl
mddl_SOURCES =
mddl_SOURCES += mddl-core.c
mddl_SOURCES += mddl-cmd-print.c
mddl_SOURCES += mddl-cmd-parse.c
mddl_CPPFLAGS = $(AM_CPPFLAGS) $(LIBXML_CFLAGS)
mddl_LDADD = libmddl.la
mddl_LDFLAGS = $(AM_LDFLAGS) $(LIBXML_LIBS)
BUILT_SOURCES += mddl-tag.c mddl-attr.c
BUILT_SOURCES += mddl-3.0-beta-spec.xml
BUILT_SOURCES += mddl-3.0-beta-spec.h
BUILT_SOURCES += mddl-3.0-beta-funs.h mddl-3.0-beta-funs.c
BUILT_SOURCES += mddl-3.0-beta-tag.gperf mddl-3.0-beta-tag.c
BUILT_SOURCES += mddl-3.0-beta-attr.gperf mddl-3.0-beta-attr.c
BUILT_SOURCES += mddl-3.0-beta-print.c
BUILT_SOURCES += mddl-3.0-beta-parse.c

mddl-3.0-beta-spec.xml: $(top_srcdir)/xsl/spec-to-struct.xsl mddl-3.0-beta-lite.xsd
	xsltproc $+ > $@

## rule for mddl-lite.h
mddl-3.0-beta-spec.h: $(top_srcdir)/xsl/autospec-lite.xsl mddl-3.0-beta-spec.xml
	xsltproc $+ > $@

mddl-3.0-beta-funs.h: $(top_srcdir)/xsl/autofuns-lite.xsl mddl-3.0-beta-spec.xml
	xsltproc --stringparam hdr yes $+ > $@

mddl-3.0-beta-funs.c: $(top_srcdir)/xsl/autofuns-lite.xsl mddl-3.0-beta-spec.xml
	xsltproc $+ > $@

mddl-3.0-beta-tag.gperf: $(top_srcdir)/xsl/autogperf-tag.xsl mddl-3.0-beta-spec.xml
	xsltproc $+ > $@

mddl-3.0-beta-attr.gperf: $(top_srcdir)/xsl/autogperf-attr.xsl mddl-3.0-beta-spec.xml
	xsltproc $+ > $@

mddl-3.0-beta-print.c: $(top_srcdir)/xsl/autoprint.xsl mddl-3.0-beta-spec.xml
	xsltproc $+ > $@

mddl-3.0-beta-parse.c: $(top_srcdir)/xsl/autoparse.xsl mddl-3.0-beta-spec.xml
	xsltproc $+ > $@

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
