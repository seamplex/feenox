SUBDIRS = src

dist_doc_DATA = AUTHORS \
                TODO \
                ChangeLog \
                COPYING \
                README \
                README4engineers \
                README4hackers \
                README4academics \
                doc/programming \
                doc/compilation \
                doc/FAQ \
                doc/CODE_OF_CONDUCT \
                doc/srs \
                doc/sds \
                doc/feenox-manual \
                doc/feenox.xml \
                doc/fee.vim
                
dist_man_MANS = doc/feenox.1


TESTS = \
        tests/abort.sh \
        tests/algebraic_expr.sh \
        tests/annulus-modal.sh \
        tests/uo2-pellet.sh \
        tests/arguments.sh \
        tests/azmy.sh \
        tests/beam-modal.sh \
        tests/beam-ortho.sh \
        tests/bimetallic-strip.sh \
        tests/bunny-diffusion.sh \
        tests/bunny-modal.sh \
        tests/bunny-sn.sh \
        tests/bunny-thermal.sh \
        tests/builtin.sh \
        tests/cog.sh \
        tests/cylinder-traction-force.sh \
        tests/encased_rod.sh \
        tests/expressions.sh \
        tests/expressions_constants.sh \
        tests/exp.sh \
        tests/i-beam-euler-bernoulli.sh \
        tests/iaea-pwr.sh \
        tests/iterative.sh \
        tests/fit.sh \
        tests/file.sh \
        tests/function_algebraic.sh \
        tests/function_data.sh \
        tests/function_file.sh \
        tests/function_mesh.sh \
        tests/function_vectors.sh \
        tests/hello_mpi.sh \
        tests/integral.sh \
        tests/laplace2d.sh \
        tests/los-alamos.sh \
        tests/map-cube.sh \
        tests/materials.sh \
        tests/mesh.sh \
        tests/moment-of-inertia.sh \
        tests/nafems-le1.sh \
        tests/nafems-le10.sh \
        tests/nafems-le11.sh \
        tests/nafems-t1-4.sh \
        tests/nafems-t2-3.sh \
        tests/neutron_diffusion_src.sh \
        tests/neutron_diffusion_keff.sh \
        tests/neutron-fully-mirrored.sh \
        tests/parallelepiped.sh \
        tests/petsc_options.sh \
        tests/pipe.sh \
        tests/point-kinetics.sh \
        tests/print.sh \
        tests/ray-effect.sh \
        tests/reactions.sh \
        tests/reactions-elastic.sh \
        tests/reed.sh \
        tests/solve.sh \
        tests/spinning-disk.sh \
        tests/statically_indeterminate_reaction.sh \
        tests/symmetry.sh \
        tests/t21.sh \
        tests/thermal-1d.sh \
        tests/thermal-2d.sh \
        tests/thermal-3d.sh \
        tests/thermal-slab-no-k.sh \
        tests/thermal-slab-wrong-bc.sh \
        tests/thermal-radiation.sh \
        tests/transient-mesh.sh \
        tests/trig.sh \
        tests/two-cubes-isotropic.sh \
        tests/two-cubes-orthotropic.sh \
        tests/vector.sh \
        tests/xfail-few-properties-ortho-young.sh \
        tests/xfail-few-properties-ortho-poisson.sh \
        tests/xfail-few-properties-ortho-shear.sh \
        tests/wilson.sh

XFAIL_TESTS = tests/abort.sh \
              tests/thermal-slab-no-k.sh \
              tests/thermal-slab-wrong-bc.sh \
              tests/xfail-few-properties-ortho-young.sh \
              tests/xfail-few-properties-ortho-poisson.sh \
              tests/xfail-few-properties-ortho-shear.sh

# when getting
# ERROR: files left in build directory after distclean:
# when creating the source dist, add them here

DISTCLEANFILES = \
                 config.guess \
                 compile \
                 depcomp \
                 install-sh \
                 config.sub \
                 test-driver \
                 missing \
                 feenox \
                 tests/function_vectors1d.last \
                 tests/nafems-le10.msh \
                 tests/fit1d_gradient.last \
                 tests/fibo_vector.last \
                 tests/fibo_formula.last \
                 tests/expressions_variables.last \
                 tests/nafems-le10.last \
                 tests/square-centered.msh \
                 tests/2dpwr-eighth.msh \
                 tests/nafems-t4.msh \
                 tests/find_extrema2d.last \
                 tests/algebraic_expr.last \
                 tests/2dpwr-quarter.msh \
                 tests/fibo_iterative.last \
                 tests/parallelepiped.msh \
                 tests/print_function.last \
                 tests/i-beam-tet.msh \
                 tests/print_vector.last \
                 tests/i-beam-hex.msh \
                 tests/iterative.last \
                 mesh2d.msh \
                 laplace-square.msh \
                 square-f.msh \
                 map-cube-4-10-dst.msh \
                 tests/transient-from-mesh-same-dt.last \
                 tests/transient-to-mesh.last \
                 tests/cube.msh \
                 tests/transient-from-mesh-different-dt.last \
                 map-cube-4-src.msh \
                 map-cube-10-4-dst.msh \
                 map-cube-10.msh \
                 map-cube-4.msh \
                 map-cube-10-src.msh \
                 two-cubes-orthotropic-variables.vtk \
                 nafems-le1-5.vtk \
                 parallelepiped-thermal.msh \
                 long-bar-unstruct-temperature.msh \
                 two-cubes-orthotropic-functions.vtk \
                 tests/reaction-elastic-lt.last \
                 tests/spinning-disk-parallel-solid-halfquarter_unstruct.msh \
                 tests/reaction-displ.last \
                 tests/nafems-le11-tet4.msh \
                 tests/pellet.msh \
                 tests/wilson-2d.msh \
                 tests/nafems-le11-hex27.msh \
                 tests/two-cubes.msh \
                 tests/spinning-disk-parallel-plane-quarter_unstruct.msh \
                 tests/spinning-disk-parallel-plane-eighth_unstruct.msh \
                 tests/reaction-elastic-lr.last \
                 tests/spinning-disk-parallel-solid-halfeighth_unstruct.msh \
                 tests/long-bar-struct.msh \
                 tests/reaction-force.last \
                 tests/nafems-t1.msh \
                 tests/long-bar-unstruct.msh \
                 tests/encased_rod.msh \
                 tests/spinning-disk-parallel-plane-sixteenth_unstruct.msh \
                 tests/nafems-le11-tet10.msh \
                 tests/statically_indeterminate_reaction.msh \
                 tests/wilson-1d.msh \
                 tests/spinning-disk-parallel-solid-halfsixteenth_unstruct.msh \
                 tests/nafems-le11-hex8.msh \
                 tests/nafems-le11-hex20.msh \
                 nafems-le1-4.vtk \
                 nafems-le11-hex27.vtk \
                 nafems-le1-1.vtk \
                 nafems-le1-2.vtk \
                 long-bar-struct-temperature.msh \
                 nafems-le11-hex20.vtk \
                 nafems-le1-6.vtk \
                 parallelepiped-coarse.msh \
                 nafems-le1-8.vtk \
                 encased_rod_tb.vtk \
                 encased_rod_lb.vtk \
                 two-cubes-isotropic-materials.vtk \
                 nafems-t4-result.msh \
                 nafems-le1-3.vtk \
                 nafems-le11-tet10.vtk \
                 two-cubes-isotropic-functions.vtk \
                 nafems-le11-hex8.vtk \
                 two-cubes-orthotropic-materials.vtk \
                 two-cubes-isotropic-variables.vtk \
                 nafems-le1-7.vtk \
                 nafems-le11-tet4.vtk \
                 tests/annulus-modal.last \
                 tests/annulus.msh \
                 cube-radial.vtk \
                 mesh2d.vtk \
                 ./tests/expressions_cumbersome.last \
                 ./tests/steps.last \
                 ./tests/printf.last \
                 ./square_tmp.vtk \
                 ./two-is-2.txt                 

all-local:
	cp -r src/feenox$(EXEEXT) .

# clean-local: code-coverage-clean
# distclean-local: code-coverage-dist-clean
