AC_INIT([BahChumbug], [0.1], [bahchumbug@matmrosko.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PROG_CXX
AC_PROG_RANLIB

AC_CHECK_LIB([resolv], [__b64_ntop], , [NO_RESOLV])
if test ! "x$NO_RESOLV" = "x"; then
	AC_MSG_ERROR([This package requires libresolv for base-64 decode, which your system is missing])
fi
AC_CHECK_HEADERS([resolv.h])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	tinyxml/Makefile
])

AC_OUTPUT