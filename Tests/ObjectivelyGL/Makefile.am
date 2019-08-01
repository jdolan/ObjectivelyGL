AM_TESTS_ENVIRONMENT = \
	AM_TESTS=1; export AM_TESTS;

DEFAULT_INCLUDES = \
	$(top_srcdir)/Sources

TESTS = \
	Buffer \
	Program \
	Shader \
	Vector \
	VertexArray

CFLAGS += \
	-I$(top_srcdir)/Sources \
	-DRESOURCES=\"$(top_srcdir)/Resources\" \
	@HOST_CFLAGS@ \
	@CHECK_CFLAGS@ \
	@SDL2_CFLAGS@

LDADD = \
	$(top_srcdir)/Sources/ObjectivelyGL/libObjectivelyGL.la \
	@HOST_LIBS@ \
	@CHECK_LIBS@ \
	@SDL2_LIBS@

check_PROGRAMS = \
	$(TESTS)
