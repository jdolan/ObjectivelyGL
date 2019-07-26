noinst_PROGRAMS = \
	Hello

CFLAGS += \
	-I$(top_srcdir)/Sources \
	@HOST_CFLAGS@ \
	@OBJECTIVELY_CFLAGS@ \
	@SDL2_CFLAGS@

LDADD = \
	$(top_builddir)/Sources/ObjectivelyGL/libObjectivelyGL.la \
	@HOST_LIBS@ \
	@OBJECTIVELY_LIBS@ \
	@SDL2_LIBS@ \
	-lm
