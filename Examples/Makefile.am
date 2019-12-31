noinst_PROGRAMS = \
	Parallax \
	Teapot

CFLAGS += \
	-I$(top_srcdir)/Sources \
	@HOST_CFLAGS@ \
	@OBJECTIVELY_CFLAGS@ \
	@SDL2_CFLAGS@

LDADD = \
	$(top_srcdir)/Sources/ObjectivelyGL/libObjectivelyGL.la \
	@HOST_LIBS@ \
	@OBJECTIVELY_LIBS@ \
	@SDL2_LIBS@ \
	-lm
