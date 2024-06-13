#!/bin/bash

for i in feenox touch awk pandoc makeinfo yq grep cut xargs; do
 if [ -z "$(which $i)" ]; then
  echo "error: ${i} not installed"
  exit 1
 fi
done

for i in basic daes laplace thermal mechanical modal neutron_diffusion neutron_sn; do
  cp ${i}.yaml ${i}.md
done

for i in hello          \
         pi             \
         inflation      \
         lorenz         \
         logistic       \
         double         \
         boiling-2010-eta   \
         boiling-2012       \
         thermal-1d-dirichlet-uniform-k \
         nafems-le10    \
         nafems-le11    \
         nafems-le1     \
         maze           \
         maze-tran-td   \
         maze-tran-bu   \
         fibo_formula   \
         fibo_vector    \
         fibo_iterative \
         derivative     \
         cantilever     \
         fork           \
         iaea-2dpwr     \
         iaea-3dpwr     \
         reed           \
         azmy-structured  \
         azmy             \
         azmy-full        \
         cubesphere     \
         two-zone-slab  \
         parallelepiped-thermal     \
         parallelepiped-mechanical  \
         temp-cylinder-tran         \
         wire                       \
         asme-expansion             \
         cube-orthotropic-expansion \
         veeder                     \
         mechanical-square-temperature \
         two-cubes-thermal    \
         two-cubes-mechanical \
         reactivity-from-table \
         inverse-integral      \
         inverse-dae           \
         xenon                 \
         point                 \
         wing                  \
  ; do
  in=${i}.fee
  out=$(grep category ${in} | cut -d: -f2 | xargs).md
  echo ${i} '->' ${out}
  
  # the actual markdown
  awk -f extract_yaml.awk ${in} | yq -r .intro | \
    pandoc -t markdown   --lua-filter=../doc/include-files.lua \
                         --lua-filter=../doc/include-code-files.lua \
                         --lua-filter=../doc/not-in-format.lua \
                         --lua-filter=../doc/only-in-format.lua \
                         --lua-filter=../doc/img-width.lua >> ${out}
  echo >> ${out}
  
  echo >> ${out}
  echo '```feenox' >> ${out}
  awk -f extract_fee.awk ${in} >> ${out}
  echo '```' >> ${out}
  echo >> ${out}

  
  echo >> ${out}
  echo '```terminal' >> ${out}
  awk -f extract_yaml.awk ${in} | yq -r .terminal >> ${out}
  echo '```' >> ${out}
  echo >> ${out}
  
  echo >> ${out}
  awk -f extract_yaml.awk ${in} | yq -r .figures | grep -v null >> ${out}
  echo >> ${out}
done

rm -f examples.md
for i in basic daes laplace thermal mechanical modal neutron_diffusion neutron_sn; do
  echo "- $(yq -r .title ${i}.yaml)" >> examples.md
  # shift-heading-level does not work
  pandoc ${i}.md -t commonmark --toc  --template=template_toc.md 2> /dev/null | sed s!\(\#!\(https://seamplex.com/feenox/examples/${i}.html\#! | sed 's/- /  - /'  >> examples.md
#   ../doc/md2.sh --pdf  ${i}.md
  ../doc/md2.sh --gfm  ${i}.md
  ../doc/md2.sh --html ${i}.md
done


../doc/md2.sh --gfm  README.md
../doc/md2.sh --html README.md
