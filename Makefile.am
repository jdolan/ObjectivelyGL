ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	Sources \
	Resources \
	Tests \
	Examples

html:
	doxygen
#install -m 644 Documentation/*.{gif,jpg} Documentation/html

html-clean:
	rm -rf Documentation/html

html-release:
	rsync -rz --delete Documentation/html/ jaydolan.com:/var/www/projects/ObjectivelyGL
