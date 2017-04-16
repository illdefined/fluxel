ACLOCAL_AMFLAGS = -I m4

fluxel_CFLAGS = $(systemd_CFLAGS)
fluxel_LDADD = $(systemd_LIBS)

libexec_PROGRAMS = fluxel
fluxel_SOURCES = fluxel.c

systemdsystemunit_DATA = fluxel.service fluxel.socket

fluxel.service: fluxel.service.in
	sed 's|@libexecdir[@]|@libexecdir@|' $^ >$@

SUBDIR = test
