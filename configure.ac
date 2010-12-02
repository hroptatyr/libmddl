AC_PREREQ([2.61])
AC_INIT([milf], [0.1], [http://www.ga-group.nl/])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([src/config.h])
AC_CONFIG_SRCDIR([src/])

AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_YACC
AM_PROG_LEX

LT_INIT

PKG_CHECK_MODULES([MXML], [mxml >= 2.6], [:], [:])
PKG_CHECK_MODULES([LIBXML], [libxml-2.0 >= 2.7])
PKG_CHECK_MODULES([LUA], [lua >= 5.1.0], [:], [:])

## output
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_OUTPUT

