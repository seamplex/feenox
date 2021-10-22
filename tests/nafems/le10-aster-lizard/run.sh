#!/bin/bash

# check for needed tools
for i in grep awk gmsh jq /usr/bin/time; do
 if [ -z "$(which $i)" ]; then
  echo "error: $i not installed"
  exit 1
 fi
done

time="/usr/bin/time -f %e\t%S\t%U\t%M\t"
# fully serial tests
export OMP_NUM_THREADS=1

# check for codes
# --- FeenoX ----------------------
if [ ! -z "$(which feenox)" ]; then
  has_feenox="yes"
  rm -f feenox-*.dat
  $(feenox -V | grep mumps > /dev/null)
  if [ $? -eq 0 ]; then
    has_feenox_mumps="yes"
    rm -f feenox_mumps-*.dat
  fi
fi

# --- Sparselizard ----------------------
if [ -e "sparselizard/sparselizard" ]; then
  has_sparselizard="yes"
  rm -f sparselizard-*.dat
fi

# --- Code Aster ----------------------
if [ ! -z "$(which as_run)" ]; then
  has_aster="yes"
  rm -f aster-*.dat
fi

# --- Reflex ----------------------
if [ ! -z "$(which reflexCLI)" ]; then
  has_reflex="yes"
  rm -f reflex-*.dat
fi

# TODO: --check

for c in 1 0.5; do
 for m in hex tet; do

  if [ ! -e le10-${m}-${c}.msh ]; then
    gmsh -3 le10-${m}.geo -clscale ${c} -o le10-${m}-${c}.msh || exit 1
    gmsh -3 le10-${m}-${c}.msh -setnumber Mesh.SecondOrderIncomplete 1 -order 2 -o le10_2nd-${m}-${c}.msh || exit 1
  fi

  # ---- FeenoX (GAMG) -----------------------------------------
  if [ ! -z "${has_feenox}" ]; then
    if [ ! -e feenox-${m}-${c}.sigmay ]; then
      echo "running FeenoX GAMG m = ${m} c = ${c}"
      ${time} -o feenox-${m}-${c}.time feenox le10.fee ${m}-${c} > feenox-${m}-${c}.sigmay || exit 1
    fi
    echo -ne "${c}\t" >> feenox-${m}.dat
    cat feenox-${m}-${c}.sigmay | tr "\n" "\t" >> feenox-${m}.dat
    cat feenox-${m}-${c}.time   | tr "\n" "\t" >> feenox-${m}.dat
    echo >> feenox-${m}.dat
  fi

  # ---- FeenoX (MUMPS) -----------------------------------------
  if [ ! -z "${has_feenox_mumps}" ]; then
    if [ ! -e feenox_mumps-${m}-${c}.sigmay ]; then
      echo "running FeenoX MUMPS c = ${c}"
      ${time} -o feenox_mumps-${m}-${c}.time feenox le10.fee ${m}-${c} --mumps > feenox_mumps-${m}-${c}.sigmay || exit 1
    fi
    echo -ne "${c}\t" >> feenox_mumps-${m}.dat
    cat feenox_mumps-${m}-${c}.sigmay | tr "\n" "\t" >> feenox_mumps-${m}.dat
    cat feenox_mumps-${m}-${c}.time   | tr "\n" "\t" >> feenox_mumps-${m}.dat
    echo >> feenox_mumps-${m}.dat
  fi
  
  # ---- Sparselizard ----------------------------------
  if [ ! -z "${has_sparselizard}" ]; then
  
    if [ ! -e sparselizard-${m}-${c}.sigmay ]; then
      echo "Running Sparselizard m = ${m} c = ${c}"
      cd sparselizard
      ${time} -o ../sparselizard-${m}-${c}.time ./run_sparselizard.sh ${m}-${c} > ../sparselizard-${m}-${c}.sigmay || exit 1
      cd ..
    fi
    echo -ne "${c}\t" >> sparselizard-${m}.dat
    cat sparselizard-${m}-${c}.sigmay | tr "\n" "\t" >> sparselizard-${m}.dat
    cat sparselizard-${m}-${c}.time   | tr "\n" "\t" >> sparselizard-${m}.dat
    echo >> sparselizard-${m}.dat
  fi
  

  
  # ---- Code Aster ----------------------------------
  if [ ! -z "${has_aster}" ]; then
    if [ ! -e le10_2nd-${m}-${c}.unv ]; then
      gmsh -3 le10_2nd-${m}-${c}.msh -o le10_2nd-${m}-${c}.unv || exit 1
    fi
    if [ ! -e aster-${m}-${c}.sigmay ]; then
      echo "running Aster c = ${c}"
      cp le10-${c}_2nd.unv le10_2nd.unv
      ${time} -o aster-${m}-${c}.time as_run le10.export || exit 1
      grep "degrés de liberté:" message  | awk '{printf("%g\t", $7)}' > aster-${m}-${c}.sigmay
      grep "2.00000000000000E+03  0.00000000000000E+00  3.00000000000000E+02" DD.txt | awk '{print $5}' >> aster-${m}-${c}.sigmay
    fi
    echo -ne "${c}\t" >> aster-${m}.dat
    cat le10-${m}-${c}.nodes    | tr "\n" "\t" >> aster-${m}.dat
    cat aster-${m}-${c}.sigmay | tr "\n" "\t" >> aster-${m}.dat
    cat aster-${m}-${c}.time   | tr "\n" "\t" >> aster-${m}.dat
    echo >> aster-${m}.dat


  fi

  # ---- Reflex ----------------------------------
  if [ ! -z "${has_reflex}" ]; then
    if [ ! -e reflex-${m}-${c}.sigmay ]; then
      echo "running Reflex c = ${c}"
      ${time} -o reflex-${m}-${c}.time reflexCLI -i le10.json -s "c=${m}-${c}" || exit 1
      grep "degrees of freedom" output/le10.log | awk '{printf("%g\t", $3)}' > reflex-${m}-${c}.sigmay
      jq .outputs.kpi_data[0].symm_tensor.data[0].yy output/le10_result_kpi.json >> reflex-${m}-${c}.sigmay
    fi
    echo -ne "${c}\t" >> reflex-${m}.dat
    cat reflex-${m}-${c}.sigmay | tr "\n" "\t" >> reflex-${m}.dat
    cat reflex-${m}-${c}.time   | tr "\n" "\t" >> reflex-${m}.dat
    echo >> reflex-${m}.dat
  fi
  
  
 done
done
