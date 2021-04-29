SUBDIRS = src doc

dist_doc_DATA = AUTHORS ChangeLog README TODO \
                doc/feenox.texi doc/logo.svg doc/feenox.xml doc/reference.md

# dist_man_MANS = doc/feenox.1

EXTRA_DIST = examples tests locateruntest.sh

TESTS = tests/abort.sh \
        tests/builtin.sh \
        tests/default_argument_value.sh \
        tests/expressions_functions.sh \
        tests/expr.sh \
        tests/exp.sh \
        tests/function_algebraic.sh \
        tests/integral.sh \
        tests/materials.sh \
        tests/mesh.sh \
        tests/thermal-1d.sh \
        tests/thermal-2d.sh \
        tests/trig.sh

all-local:
	cp -r src/feenox$(EXEEXT) .
