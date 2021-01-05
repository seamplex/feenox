SUBDIRS = src doc

dist_doc_DATA = AUTHORS ChangeLog README TODO \
                doc/feenox.texi doc/feenox.info doc/logo.svg doc/feenox.xml doc/reference.md

# dist_man_MANS = doc/feenox.1

EXTRA_DIST = examples tests locateruntest.sh

# TESTS = tests/fibonacci.sh \
#         tests/pi.sh \
#         tests/interp1d.sh \
#         tests/lorenz.sh

all-local:
	cp -r src/feenox$(EXEEXT) .
