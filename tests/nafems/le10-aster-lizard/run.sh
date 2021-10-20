#!/bin/bash

# check for needed tools
for i in grep awk gmsh jq /usr/bin/time; do
 if [ -z "$(which $i)" ]; then
  echo "error: $i not installed"
  exit 1
 fi
done

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

for c in 1 0.5 0.2; do
 for m in hex tet; do

  if [ ! -e le10-${m}-${c}.msh ]; then
    gmsh -3 le10-${m}.geo -clscale ${c} -o le10-${m}-${c}.msh || exit 1
  fi

  # count nodes
  # TODO: use the reported number of total DOFs from each code
  gmsh - -v 0 le10-${m}-${c}.msh nodes.geo || exit 1
  mv nodes.txt le10-${m}-${c}.nodes
  
  # ---- FeenoX (GAMG) -----------------------------------------
  if [ ! -z "${has_feenox}" ]; then
    if [ ! -e feenox-${m}-${c}.sigmay ]; then
      echo "running FeenoX GAMG m = ${m} c = ${c}"
      /usr/bin/time -f "%e\t%S\t%U\t%M " -o feenox-${m}-${c}.time feenox le10.fee ${m}-${c} > feenox-${m}-${c}.sigmay || exit 1
    fi
    echo -ne "${c}\t" >> feenox-${m}.dat
#     cat le10-${m}-${c}.nodes    | tr "\n" "\t" >> feenox-${m}.dat
    cat feenox-${m}-${c}.sigmay | tr "\n" "\t" >> feenox-${m}.dat
    cat feenox-${m}-${c}.time   | tr "\n" "\t" >> feenox-${m}.dat
    echo >> feenox-${m}.dat
  fi

  # ---- FeenoX (MUMPS) -----------------------------------------
  if [ ! -z "${has_feenox_mumps}" ]; then
    if [ ! -e feenox_mumps-${m}-${c}.sigmay ]; then
      echo "running FeenoX MUMPS c = ${c}"
      /usr/bin/time -f "%e\t%S\t%U\t%M " -o feenox_mumps-${m}-${c}.time feenox le10.fee ${m}-${c} --mumps > feenox_mumps-${m}-${c}.sigmay || exit 1
    fi
    echo -ne "${c}\t" >> feenox_mumps-${m}.dat
    cat le10-${m}-${c}.nodes    | tr "\n" "\t" >> feenox_mumps-${m}.dat
    cat feenox_mumps-${m}-${c}.sigmay | tr "\n" "\t" >> feenox_mumps-${m}.dat
    cat feenox_mumps-${m}-${c}.time   | tr "\n" "\t" >> feenox_mumps-${m}.dat
    echo >> feenox_mumps-${m}.dat
  fi
  
  # ---- Sparselizard ----------------------------------
  if [ ! -z "${has_sparselizard}" ]; then
  
    # it does not like quad8
    if [ "x${m}" = "xstr_hex20" ]; then
      if [ ! -e le10-${m}-${c}-first.msh ]; then
        gmsh -3 le10-${m}-${c}.msh -order 1 -o le10-${m}-${c}-first.msh || exit 1
      fi  
    fi
  
    if [ ! -e sparselizard-${m}-${c}.sigmay ]; then
      echo "Running Sparselizard m = ${m} c = ${c}"
      cd sparselizard
      /usr/bin/time -f "%e\t%S\t%U\t%M " -o ../sparselizard-${m}-${c}.time ./run_sparselizard.sh ${m}-${c} > ../sparselizard-${m}-${c}.sigmay || exit 1
      cd ..
    fi
    echo -ne "${c}\t" >> sparselizard-${m}.dat
    cat le10-${m}-${c}.nodes    | tr "\n" "\t" >> sparselizard-${m}.dat
    cat sparselizard-${m}-${c}.sigmay | tr "\n" "\t" >> sparselizard-${m}.dat
    cat sparselizard-${m}-${c}.time   | tr "\n" "\t" >> sparselizard-${m}.dat
    echo >> sparselizard-${m}.dat
  fi
  
  if [ ! -z "${has_aster}" ]; then
    if [ ! -e le10-${m}-${c}.unv ]; then
      gmsh -3 le10-${m}-${c}.msh -o le10-${m}-${c}.unv || exit 1
    fi
    if [ ! -e aster-${m}-${c}.sigmay ]; then
      echo "running Aster c = ${c}"
      cp le10-${c}.unv le10.unv
      /usr/bin/time -f "%e\t%S\t%U\t%M " -o aster-${m}-${c}.time as_run le10.export || exit 1
      grep "2.00000000000000E+03  0.00000000000000E+00  3.00000000000000E+02" DD.txt | awk '{print $5}' > aster-${m}-${c}.sigmay
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
      /usr/bin/time -f "%e\t%S\t%U\t%M " -o reflex-${m}-${c}.time reflexCLI -i le10.json -s "c=${m}-${c}" || exit 1
      jq .outputs.kpi_data[0].symm_tensor.data[0].yy output/le10_result_kpi.json > reflex-${m}-${c}.sigmay
    fi
    echo -ne "${c}\t" >> reflex-${m}.dat
    cat le10-${m}-${c}.nodes    | tr "\n" "\t" >> reflex-${m}.dat
    cat reflex-${m}-${c}.sigmay | tr "\n" "\t" >> reflex-${m}.dat
    cat reflex-${m}-${c}.time   | tr "\n" "\t" >> reflex-${m}.dat
    echo >> reflex-${m}.dat
    
  fi
 done
done
