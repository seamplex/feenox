#!/bin/bash

rm -f README.md
for i in lorenz; do
  echo ${i}
  grep '#\.' ${i}.fee | sed 's/#.//' | sed 's/^[ \t]*//' | \
    pandoc -t markdown   --lua-filter=../doc/include-files.lua \
                         --lua-filter=../doc/include-code-files.lua \
                         --lua-filter=../doc/not-in-format.lua >> README.md
  echo >> README.md
  
  grep '#\$' ${i}.fee | sed 's/#$//' | sed 's/^[ \t]*//' >> ${i}-post.sh
  bash ${i}-post.sh
#   rm -f ${i}-post.sh

  echo >> README.md
  echo '```feenox' >> README.md
  cat ${i}.fee | grep -v '#\.' | grep -v '#\$' | grep -v '#>' >> README.md
  echo '```' >> README.md
  echo >> README.md

  grep '#>' ${i}.fee | sed 's/#>//' | sed 's/^[ \t]*//' >> README.md
  echo >> README.md
done
