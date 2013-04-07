AC_INIT([evremote2],[1.0],[],[evremote2])
AC_PREREQ(2.53)

AM_INIT_AUTOMAKE

AC_PROG_CC
AC_PROG_CXX

m4_ifdef([LT_INIT], [LT_INIT], [AC_PROG_LIBTOOL])
AC_CONFIG_MACRO_DIR([m4])

AC_CHECK_LIB(pthread, pthread_create, [LIBS="$LIBS -lpthread"])

AC_OUTPUT([
Makefile
])
