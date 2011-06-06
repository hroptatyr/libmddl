AM_CFLAGS = -std=c99
AM_CPPFLAGS = -Wall -DDEBUG -D_GNU_SOURCE

headerdir = $(includedir)/mddl
stubdir = $(libexecdir)/mddl

bin_PROGRAMS =
lib_LTLIBRARIES =
header_HEADERS = 
stub_PROGRAMS =
stub_LIBRARIES =
BUILT_SOURCES =
CLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST =

lib_LTLIBRARIES += libmddl.la
libmddl_la_SOURCES = mddl.c mddl.h
libmddl_la_CPPFLAGS = $(AM_CPPFLAGS) $(LIBXML_CFLAGS)

bin_PROGRAMS += mddl
mddl_SOURCES =
mddl_SOURCES += mddl-core.c mddl-core.h
mddl_SOURCES += mddl-cmd-print.c
mddl_SOURCES += mddl-cmd-parse.c
mddl_SOURCES += mddl-cmd-code.c
mddl_SOURCES += mddl-cmd-name.c
mddl_SOURCES += mddl-cmd-objective.c
mddl_CPPFLAGS = $(AM_CPPFLAGS) $(LIBXML_CFLAGS)
mddl_LDADD = libmddl.la
mddl_LDFLAGS = -static $(LIBXML_LIBS)
BUILT_SOURCES += mddl-3.0-beta-spec.xml
BUILT_SOURCES += mddl-3.0-beta-spec.h
BUILT_SOURCES += mddl-3.0-beta-funs.h mddl-3.0-beta-funs.c
BUILT_SOURCES += mddl-3.0-beta-tag.gperf mddl-3.0-beta-tag.c
BUILT_SOURCES += mddl-3.0-beta-attr.gperf mddl-3.0-beta-attr.c
BUILT_SOURCES += mddl-3.0-beta-print.c
BUILT_SOURCES += mddl-3.0-beta-parse.c
EXTRA_DIST += mddl-3.0-beta-lite.xsd
header_HEADERS += mddl.h mddl-3.0-beta-spec.h mddl-3.0-beta-funs.h

stub_LIBRARIES += libmddl_stub.a
libmddl_stub_a_SOURCES =
libmddl_stub_a_SOURCES += mddl-cmd-print.c
libmddl_stub_a_SOURCES += mddl-cmd-parse.c
libmddl_stub_a_SOURCES += mddl-cmd-code.c
libmddl_stub_a_SOURCES += mddl-cmd-name.c
libmddl_stub_a_SOURCES += mddl-cmd-objective.c
header_HEADERS += mddl-core.h


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
