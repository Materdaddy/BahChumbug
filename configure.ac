AC_INIT([BahChumbug], [0.1], [bahchumbug@matmrosko.com])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PROG_CXX

AC_CHECK_LIB([resolv], [__b64_ntop], , [NO_RESOLV=1])
if test ! "x$NO_RESOLV" = "x"; then
	AC_MSG_ERROR([BahChumbug requires libresolv for base-64 decode, which your system is missing])
fi
AC_CHECK_HEADERS([resolv.h])

AC_CHECK_LIB([mpg123], [mpg123_init], , [NO_MPG123=yes])
if test x"$NO_MPG123" = x"yes"; then
	AC_MSG_ERROR([BahChumbug requires libmpg123 for mp3 decoding, which your system is missing])
fi
AC_CHECK_HEADERS([mpg123.h])

AC_CHECK_LIB([asound], [snd_pcm_open], , [NO_ASOUND=yes])
if test x"$NO_ASOUND" = x"yes"; then
	AC_MSG_ERROR([BahChumbug requires alsa (libasound) for audio playback, which your system is missing])
fi
AC_CHECK_HEADERS([alsa/asoundlib.h])

AC_CHECK_LIB([pthread], [pthread_create], , [NO_PTHREAD=yes])
if test x"$NO_PTHREAD" = x"yes"; then
	AC_MSG_ERROR([BahChumbug requires libpthread to create sound thread, which your system is missing])
fi
AC_CHECK_HEADERS([pthread.h])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
])

AC_CONFIG_SUBDIRS([tinyxml])
AC_OUTPUT
