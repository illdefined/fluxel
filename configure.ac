AC_PREREQ([2.60])
AC_INIT([fluxel], NY_GIT_VERSION, [mvs@nyantec.com])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([fluxel.c])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_TARGET
AX_ENABLE_BUILDDIR([$target])
AM_INIT_AUTOMAKE([foreign tar-pax dist-xz no-dist-gzip])

AC_PROG_CC([clang gcc cl cc])
AC_PROG_CC_STDC
AC_PROG_INSTALL

AC_DEFINE([_POSIX_C_SOURCE], [200809l], [Expose POSIX.1-2008 definitions.])
AC_DEFINE([_XOPEN_SOURCE], [700], [Expose XSI extensions.])
AC_DEFINE([_GNU_SOURCE], [1], [Expose GNU extensions.])

AC_HEADER_ASSERT
AC_HEADER_STDC

AC_TYPE_SIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINTPTR_T
AX_TYPE_SOCKLEN_T

AC_C_RESTRICT

AX_CFLAGS_NO_WRITABLE_STRINGS
AX_CFLAGS_WARN_ALL

AX_ADD_FORTIFY_SOURCE
AX_APPEND_COMPILE_FLAGS([ \
	-Waddress \
	-Wcast-align \
	-Wclobbered \
	-Wconversion \
	-Werror=implicit-function-declaration \
	-Werror=init-self \
	-Werror=nonnull \
	-Werror=sequence-point \
	-Werror=uninitialised \
	-Wfloat-equal \
	-Wformat \
	-Wformat-security \
	-Wmaybe-unitialised \
	-Wno-sign-compare \
	-Wpointer-sign \
	-Wredundant-decls \
	-Wshadow \
	-Wsign-conversion \
	-Wsizeof-pointer-memaccess \
	-Wstack-protector \
	-Wstrict-overflow=4 \
	-Wtrampolines \
	-Wtype-limits \
	-Wvolatile-register-var \
	-Wwrite-strings \
	-fstack-protector-strong \
	-ftrapv \
	-fPIE \
	-flto \
])

AX_APPEND_LINK_FLAGS([ \
	-pie \
	-Wl,-z,combreloc \
	-Wl,-z,relro \
	-Wl,-z,now \
])

AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([accept4 epoll_ctl])

PKG_CHECK_MODULES([systemd], [libsystemd])

AC_CONFIG_FILES([
	Makefile
	test/Makefile
])
AC_OUTPUT
