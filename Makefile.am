ACLOCAL_AMFLAGS = -I m4

pixelwut_CFLAGS = $(systemd_CFLAGS)
pixelwut_LDADD = $(systemd_LIBS)

libexec_PROGRAMS = pixelwut
pixelwut_SOURCES = pixelwut.c

systemdsystemunit_DATA = pixelwut.service pixelwut.socket

pixelwut.service: pixelwut.service.in
	sed 's|@libexecdir[@]|@libexecdir@|' $^ >$@

SUBDIR = test
