SUBDIRS = src doc

dist_doc_DATA = AUTHORS ChangeLog README TODO \
                doc/feenox.texi doc/feenox.info doc/logo.svg doc/feenox.xml doc/reference.md

# dist_man_MANS = doc/feenox.1

EXTRA_DIST = examples tests locateruntest.sh

TESTS = tests/abort.sh \
        tests/expr.sh

all-local:
	cp -r src/feenox$(EXEEXT) .
