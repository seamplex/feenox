SUBDIRS = src doc

dist_doc_DATA = AUTHORS ChangeLog README TODO \
                doc/feenox.texi doc/logo.svg doc/feenox.xml doc/reference.md

dist_man_MANS = doc/feenox.1

EXTRA_DIST = examples tests locateruntest.sh

TESTS = tests/abort.sh \
        tests/algebraic_expr.sh \
        tests/builtin.sh \
        tests/default_argument_value.sh \
        tests/expressions_constants.sh \
        tests/expressions_variables.sh \
        tests/expressions_functions.sh \
        tests/exp.sh \
        tests/function_algebraic.sh \
        tests/function_data.sh \
        tests/function_vectors.sh \
        tests/integral.sh \
        tests/materials.sh \
        tests/mesh.sh \
        tests/nafems-tx.sh \
        tests/thermal-1d.sh \
        tests/thermal-2d.sh \
        tests/trig.sh

XFAIL_TESTS = tests/abort.sh         

all-local:
	cp -r src/feenox$(EXEEXT) .
