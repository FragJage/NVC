## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

if HAVE_ALSA

alsaltlibs = libalsa.la
alsasources = ao_alsa.c

else

alsaltlibs =
alsasources =

endif

INCLUDES = -I$(top_builddir)/include/ao -I$(top_srcdir)/include

libdir = $(plugindir)
lib_LTLIBRARIES = $(alsaltlibs)

libalsa_la_LDFLAGS = @PLUGIN_LDFLAGS@
libalsa_la_LIBADD = @ALSA_LIBS@ ../../libao.la
libalsa_la_SOURCES = $(alsasources)

EXTRA_DIST = ao_alsa.c
