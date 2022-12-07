#!/bin/bash

out=examples.md
rm -f ${out}
for i in hello          \
         lorenz         \
         logistic       \
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
  ; do
  echo ${i}
  grep '#\.' ${i}.fee | sed 's/#\. //' | sed 's/#\.//' | sed 's/^[\t]*//' | \
    pandoc -t markdown   --lua-filter=../doc/include-files.lua \
                         --lua-filter=../doc/include-code-files.lua \
                         --lua-filter=../doc/not-in-format.lua \
                         --lua-filter=../doc/only-in-format.lua \
                         --lua-filter=../doc/img-width.lua >> ${out}
  echo >> ${out}
  
  echo >> ${out}
  echo '```feenox' >> ${out}
  cat ${i}.fee | grep -v '#\.' | grep -v '#\$' | grep -v '#>' >> ${out}
  echo '```' >> ${out}
  echo >> ${out}

  
  echo >> ${out}
  echo '```terminal' >> ${out}
  grep '#\$' ${i}.fee | sed 's/#\$//' | sed 's/^[ \t]*//' >> ${out}
#   bash ${i}-post.sh
#   rm -f ${i}-post.sh
  echo '```' >> ${out}
  echo >> ${out}
  
  echo >> ${out}
  grep '#>' ${i}.fee | sed 's/#>//' | sed 's/^[ \t]*//' >> ${out}
  echo >> ${out}
done

../doc/md2.sh --pdf README
../doc/md2.sh --gfm README
../doc/md2.sh --html README
