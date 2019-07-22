
AM_TESTS_ENVIRONMENT = \
	AM_TESTS=1; export AM_TESTS;

DEFAULT_INCLUDES = \
	$(top_srcdir)/Sources

TESTS = \
	Program \
	Shader

CFLAGS += \
	-I$(top_srcdir)/Sources \
	@HOST_CFLAGS@ \
	@CHECK_CFLAGS@ \
	@SDL2_CFLAGS@

LDADD = \
	$(top_srcdir)/Sources/Objectively/libObjectivelyGL.la \
	@HOST_LIBS@ \
	@CHECK_LIBS@ \
	@SDL2_LIBS@

check_PROGRAMS = \
	$(TESTS)
