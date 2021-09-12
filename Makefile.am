#SUBDIRS = src doc
SUBDIRS = src

dist_doc_DATA = AUTHORS ChangeLog README TODO \
                doc/CODE_OF_CONDUCT.md \
                doc/feenox.xml \
                doc/programming.md \
                doc/FAQ.md \
                doc/syntax-kate.sh \
                doc/syntax-tex.sh

dist_man_MANS = doc/feenox.1

EXTRA_DIST = examples tests locateruntest.sh tests/functions.sh

# include $(top_srcdir)/aminclude_static.am
TESTS = tests/abort.sh \
        tests/algebraic_expr.sh \
        tests/builtin.sh \
        tests/default_argument_value.sh \
        tests/expressions_constants.sh \
        tests/expressions_variables.sh \
        tests/expressions_functions.sh \
        tests/exp.sh \
        tests/iterative.sh \
        tests/iaea-pwr.sh \
        tests/fit.sh \
        tests/function_algebraic.sh \
        tests/function_data.sh \
        tests/function_file.sh \
        tests/function_vectors.sh \
        tests/integral.sh \
        tests/laplace2d.sh \
        tests/materials.sh \
        tests/mesh.sh \
        tests/modal.sh \
        tests/nafems-lex.sh \
        tests/nafems-tx.sh \
        tests/nafems-t4.sh \
        tests/neutron_diffusion_src.sh \
        tests/neutron_diffusion_keff.sh \
        tests/point-kinetics.sh \
        tests/thermal-1d.sh \
        tests/thermal-2d.sh \
        tests/trig.sh \
        tests/vector.sh

XFAIL_TESTS = tests/abort.sh         

all-local:
	cp -r src/feenox$(EXEEXT) .

# clean-local: code-coverage-clean
# distclean-local: code-coverage-dist-clean
