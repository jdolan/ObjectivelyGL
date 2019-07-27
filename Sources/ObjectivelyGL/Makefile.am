AUTOMAKE_OPTIONS = nostdinc
AM_CPPFLAGS = -I$(top_srcdir)/Sources

noinst_HEADERS = \
	Config.h

pkgincludedir = $(includedir)/$(PACKAGE_NAME)

pkginclude_HEADERS = \
	Buffer.h \
	OpenGL.h \
	Program.h \
	Shader.h \
	Types.h \
	VertexArray.h

lib_LTLIBRARIES = \
	libObjectivelyGL.la

libObjectivelyGL_la_SOURCES = \
	Buffer.c \
	OpenGL.c \
	Program.c \
	Shader.c \
	VertexArray.c

libObjectivelyGL_la_CFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/Sources \
	@HOST_CFLAGS@ \
	@OBJECTIVELY_CFLAGS@

libObjectivelyGL_la_LDFLAGS = \
	@HOST_LDFLAGS@ \
	-shared

libObjectivelyGL_la_LIBADD = \
	@HOST_LIBS@ \
	@OBJECTIVELY_LIBS@
