SUBDIRS = src

dist_doc_DATA = AUTHORS \
                TODO \
                ChangeLog \
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

TESTS = tests/reed.sh


# TESTS = \
#         tests/abort.sh \
#         tests/algebraic_expr.sh \
#         tests/annulus-modal.sh \
#         tests/uo2-pellet.sh \
#         tests/airfoil.sh \
#         tests/arguments.sh \
#         tests/azmy.sh \
#         tests/bc-groups.sh \
#         tests/beam-modal.sh \
#         tests/beam-ortho.sh \
#         tests/bimetallic-strip.sh \
#         tests/builtin.sh \
#         tests/circle.sh \
#         tests/cog.sh \
#         tests/cylinder-traction-force.sh \
#         tests/encased_rod.sh \
#         tests/expressions.sh \
#         tests/expressions_constants.sh \
#         tests/exp.sh \
#         tests/i-beam-euler-bernoulli.sh \
#         tests/iaea-pwr.sh \
#         tests/iterative.sh \
#         tests/fit.sh \
#         tests/file.sh \
#         tests/func_min.sh \
#         tests/function_algebraic.sh \
#         tests/function_data.sh \
#         tests/function_file.sh \
#         tests/function_mesh.sh \
#         tests/function_vectors.sh \
#         tests/hello_mpi.sh \
#         tests/integral.sh \
#         tests/lag.sh \
#         tests/laplace2d.sh \
#         tests/lebesgue.sh \
#         tests/los-alamos.sh \
#         tests/map-cube.sh \
#         tests/materials.sh \
#         tests/mesh.sh \
#         tests/moment-of-inertia.sh \
#         tests/nafems-le1.sh \
#         tests/nafems-le10.sh \
#         tests/nafems-le11.sh \
#         tests/nafems-t1-4.sh \
#         tests/nafems-t2-3.sh \
#         tests/neutron_diffusion_src.sh \
#         tests/neutron_diffusion_keff.sh \
#         tests/neutron-fully-mirrored.sh \
#         tests/parallelepiped.sh \
#         tests/petsc_options.sh \
#         tests/pipe.sh \
#         tests/point-kinetics.sh \
#         tests/print.sh \
#         tests/ray-effect.sh \
#         tests/reactions.sh \
#         tests/reactions-elastic.sh \
#         tests/reed.sh \
#         tests/solve.sh \
#         tests/spinning-disk.sh \
#         tests/statically_indeterminate_reaction.sh \
#         tests/symmetry.sh \
#         tests/t21.sh \
#         tests/thermal-1d.sh \
#         tests/thermal-2d.sh \
#         tests/thermal-3d.sh \
#         tests/thermal-slab-no-k.sh \
#         tests/thermal-slab-wrong-bc.sh \
#         tests/thermal-radiation.sh \
#         tests/transient-mesh.sh \
#         tests/trig.sh \
#         tests/two-cubes-isotropic.sh \
#         tests/two-cubes-orthotropic.sh \
#         tests/vector.sh \
#         tests/xfail-few-properties-ortho-young.sh \
#         tests/xfail-few-properties-ortho-poisson.sh \
#         tests/xfail-few-properties-ortho-shear.sh \
#         tests/wilson.sh
# 
# XFAIL_TESTS = tests/abort.sh \
#               tests/thermal-slab-no-k.sh \
#               tests/thermal-slab-wrong-bc.sh \
#               tests/xfail-few-properties-ortho-young.sh \
#               tests/xfail-few-properties-ortho-poisson.sh \
#               tests/xfail-few-properties-ortho-shear.sh

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
                 ./two-is-2.txt \
                 ./circle_perimeter2.dat \
                 ./circle-1-0.4.msh      \
                 ./circle-2-0.3.msh      \
                 ./circle-1-0.7.msh      \
                 ./circle-2-0.35.msh     \
                 ./circle-1-0.5.msh      \
                 ./circle-1-0.6.msh      \
                 ./circle-2-0.7.msh      \
                 ./circle-2-0.4.msh      \
                 ./circle-1-0.3.msh      \
                 ./circle-1-0.35.msh     \
                 ./circle-2-0.6.msh      \
                 ./circle-2-0.5.msh      \
                 ./circle_perimeter1.dat                                      \
                 ./orthotropic-beam-hex27.msh                                 \
                 ./2dpwr-eighth.vtk                                           \
                 ./poisson-square.msh                                         \
                 ./ray-effect-diffusion-full.msh                              \
                 ./ray-effect-quarter-4.msh                                   \
                 ./thermal-square.vtk                                         \
                 ./orthotropic-beam-hex8.msh                                  \
                 ./tests/la-p08-PUb-1-0-SP.msh                                \
                 ./tests/square-struct.msh                                    \
                 ./tests/reed.msh                                             \
                 ./tests/pipe-ust10-2-2.msh                                   \
                 ./tests/la-p10-PUb-H2O_10-1-0-CY.msh                         \
                 ./tests/pipe-sch27-2-2.msh                                   \
                 ./tests/PF.msh                                               \
                 ./tests/la-p02-PUa-1-0-SL.msh                                \
                 ./tests/la-p06-PUb-1-0-SL.msh                                \
                 ./tests/pipe-ssh20-2-2.msh                                   \
                 ./tests/azmy-structured.msh                                  \
                 ./tests/la-p09-PUb-H2O_1-1-0-CY.msh                          \
                 ./tests/pipe-sct10-2-2.msh                                   \
                 ./tests/la-IN.msh                                            \
                 ./tests/pipe-ssh27-2-2.msh                                   \
                 ./tests/la-p48-U-2-0-SL.msh                                  \
                 ./tests/maze.msh                                             \
                 ./tests/la-p49-U-2-0-SP.msh                                  \
                 ./tests/bimetallic-strip.msh                                 \
                 ./tests/la-p03-PUa-H2O_1-1-0-SL.msh                          \
                 ./tests/pipe-sst10-2-2.msh                                   \
                 ./tests/heater-cylinder-inches.msh                           \
                 ./tests/la-p07-PUb-1-0-CY.msh                                \
                 ./tests/pipe-sch20-2-2.msh                                   \
                 ./tests/la-p04-PUa-H2O_0.5-1-0-SL.msh                        \
                 ./tests/la-p51-UAl-2-0-SL.msh                                \
                 ./tests/pipe-uct10-2-2.msh                                   \
                 ./ray-effect-full-4.msh                                      \
                 ./orthotropic-beam-tet4.msh                                  \
                 ./ray-effect-half-4.msh                                      \
                 ./spinning-disk-parallel-plane-eighth_unstruct.vtk           \
                 ./2dpwr-quarter.vtk                                          \
                 ./ray-effect-diffusion-quarter.msh                           \
                 ./ray-effect-diffusion-half.msh                              \
                 ./orthotropic-beam-hex20.msh                                 \
                 ./orthotropic-beam-tet10.msh                                 \
                 ./spinning-disk-parallel-plane-quarter_unstruct.vtk          \
                 ./reaction-force.vtk                                         \
                 ./airfoil-converged.vtk                                      \
                 ./spinning-disk-parallel-plane-sixteenth_unstruct.vtk        \
                 ./heater-cylinder-inches.vtk                                 \
                 ./ray-effect-diffusion-eighth.msh                            \
                 ./airfoil-converged.msh                                      \
                 ./ray-effect-eighth-4.msh \
                 ./tests/bc-groups1.last \
                 ./tests/bc-groups2.last \
                 ./tests/bc-groups3.last \
                 ./tests/bc-groups4.last \
                 ./bc-groups1.last \
                 ./bc-groups2.last \
                 ./bc-groups3.last \
                 ./bc-groups4.last \
                 ./tests/wilson-2d-0.vtk \
                 ./tests/wilson-2d-100.vtk \
                 ./tests/wilson-2d-10.vtk \
                 ./tests/wilson-2d-175.vtk \
                 ./tests/wilson-2d-50.vtk \
                 ./tests/wilson-2d-87.5.vtk \
                 ./wilson-2d-0.vtk \
                 ./wilson-2d-100.vtk \
                 ./wilson-2d-10.vtk \
                 ./wilson-2d-175.vtk \
                 ./wilson-2d-50.vtk \
                 ./wilson-2d-87.5.vtk
                 

all-local:
	cp -r src/feenox$(EXEEXT) .

# clean-local: code-coverage-clean
# distclean-local: code-coverage-dist-clean
