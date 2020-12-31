dnl $Id$
dnl config.m4 for extension pcon
m4_include(ax_cxx_compile_stdcxx_11.m4)
AX_CXX_COMPILE_STDCXX_11

sinclude(./autoconf/pecl.m4)
sinclude(./autoconf/php-executable.m4)

PECL_INIT([pcon])

PHP_ARG_ENABLE(pcon, whether to enable pcon, [ --enable-pcon   Enable pcon])

if test "$PHP_pcon" != "no"; then
  AC_DEFINE(HAVE_PCON, 1, [whether pcon is enabled])
  PHP_ADD_LIBRARY_WITH_PATH(z3, /usr/lib/z3, Z3_SHARED_LIBADD)
  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, 1, PCON_SHARED_LIBADD)
  PHP_SUBST(PCON_SHARED_LIBADD)
  PHP_SUBST(Z3_SHARED_LIBADD)

  PHP_NEW_EXTENSION(pcon, pcon.cc, $ext_shared)
  PHP_ADD_MAKEFILE_FRAGMENT
  PHP_INSTALL_HEADERS([ext/pcon], [php_pcon.h])
fi


