# Help the Developers and yourself. Just use the C locale and settings
# for the compilation. They can still be overriden by make LANG=<whatever>
# but that is general a not very good idea
LANG = C
LC_ALL = C

EXTRA_DIST = $(TESTS)
EXTRA_DIST += $(MT_LOG_COMPILER)
TESTS =

TEST_EXTENSIONS = .mt
MT_LOG_COMPILER = $(srcdir)/mt-test.sh
AM_MT_LOG_FLAGS = --builddir $(top_builddir)/src --hash sha1sum
LOG_COMPILER = echo

TESTS += merge.1.mt
TESTS += merge.2.mt
TESTS += merge.3.mt
TESTS += merge.4.mt
TESTS += merge.5.mt
EXTRA_DIST += merge.a.mddl
EXTRA_DIST += merge.b.mddl
EXTRA_DIST += merge.c.mddl
EXTRA_DIST += merge.d.mddl

## Makefile.am ends here
