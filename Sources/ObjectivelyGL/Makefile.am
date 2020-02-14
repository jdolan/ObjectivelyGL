AUTOMAKE_OPTIONS = nostdinc
AM_CPPFLAGS = -I$(top_srcdir)/Sources

noinst_HEADERS = \
	Config.h

pkgincludedir = $(includedir)/$(PACKAGE_NAME)

pkginclude_HEADERS = \
	Attribute.h \
	Buffer.h \
	CommandQueue.h \
	Model.h \
	OpenGL.h \
	Program.h \
	Shader.h \
	Texture.h \
	Types.h \
	UniformBuffer.h \
	VertexArray.h \
	WavefrontModel.h

lib_LTLIBRARIES = \
	libObjectivelyGL.la

libObjectivelyGL_la_SOURCES = \
	Attribute.c \
	Buffer.c \
	CommandQueue.c \
	Model.c \
	OpenGL.c \
	Program.c \
	Shader.c \
	Texture.c \
	UniformBuffer.c \
	VertexArray.c \
	WavefrontModel.c

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
