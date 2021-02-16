SUBDIRS = src doc

dist_doc_DATA = AUTHORS ChangeLog README TODO \
                doc/feenox.texi doc/feenox.info doc/logo.svg doc/feenox.xml doc/reference.md

# dist_man_MANS = doc/feenox.1

EXTRA_DIST = examples tests locateruntest.sh

TESTS = tests/abort.sh \
        tests/default_argument_value.sh \
        tests/function_algebraic.sh \
        tests/expr.sh \
        tests/expressions_functions.sh \
        tests/trig.sh

all-local:
	cp -r src/feenox$(EXEEXT) .
