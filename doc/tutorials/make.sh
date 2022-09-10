../pdf.sh README
../markdown.sh README
for i in 000-setup 110-tensile-test; do
  cd $i
  echo $i
  ../../pdf.sh README
  ../../markdown.sh README
  cd ..
done

