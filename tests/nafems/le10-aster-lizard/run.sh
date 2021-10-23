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
  rm -f feenox*.dat
  $(feenox -V | grep mumps > /dev/null)
  if [ $? -eq 0 ]; then
    has_feenox_mumps="yes"
    rm -f feenox_mumps-*.dat
  fi
fi

# --- Sparselizard ----------------------gi
if [ -e "sparselizard/sparselizard" ]; then
  has_sparselizard="yes"
  rm -f sparselizard*.dat
fi

# --- Code Aster ----------------------
if [ ! -z "$(which as_run)" ]; then
  has_aster="yes"
  rm -f aster*.dat
fi

# --- Reflex ----------------------
if [ ! -z "$(which reflexCLI)" ]; then
  has_reflex="yes"
  rm -f reflex*.dat
fi

# TODO: --check

for c in 1 0.5; do
# for c in 1 0.75 0.5 0.4 0.35 0.3 0.25 0.2 0.18 0.16 0.14 0.12 0.116 0.114 0.112 0.110 0.108 0.106 0.105 0.104 0.103 0.102 0.101 0.1 0.08 0.07 0.06 0.05; do

 for m in hex tet; do

  if [ ! -e le10-${m}-${c}.msh ]; then
    gmsh -3 le10-${m}.geo -clscale ${c} -o le10-${m}-${c}.msh || exit 1
    gmsh -3 le10-${m}-${c}.msh -setnumber Mesh.SecondOrderIncomplete 1 -order 2 -o le10_2nd-${m}-${c}.msh || exit 1
  fi

  # ---- FeenoX (GAMG) -----------------------------------------
  if [ ! -z "${has_feenox}" ]; then
    if [ ! -e feenox-${m}-${c}.sigmay ]; then
      echo "running FeenoX GAMG m = ${m} c = ${c}"
      ${time} -o feenox-${m}-${c}.time feenox le10.fee ${m}-${c} > feenox-${m}-${c}.sigmay
    fi
    
    if [ -e feenox-${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' feenox-${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> feenox-${m}.dat
        cat feenox-${m}-${c}.sigmay | tr "\n" "\t" >> feenox-${m}.dat
        cat feenox-${m}-${c}.time   | tr "\n" "\t" >> feenox-${m}.dat
        echo >> feenox-${m}.dat
      fi  
    fi
  fi  

  # ---- FeenoX (MUMPS) -----------------------------------------
  if [ ! -z "${has_feenox_mumps}" ]; then
    if [ ! -e feenox_mumps-${m}-${c}.sigmay ]; then
      echo "running FeenoX MUMPS c = ${c}"
      ${time} -o feenox_mumps-${m}-${c}.time feenox le10.fee ${m}-${c} --mumps > feenox_mumps-${m}-${c}.sigmay
    fi

    if [ -e feenox_mumps-${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' feenox_mumps-${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> feenox_mumps-${m}.dat
        cat feenox_mumps-${m}-${c}.sigmay | tr "\n" "\t" >> feenox_mumps-${m}.dat
        cat feenox_mumps-${m}-${c}.time   | tr "\n" "\t" >> feenox_mumps-${m}.dat
        echo >> feenox_mumps-${m}.dat
      fi  
    fi  
  fi
  
  # ---- Sparselizard ----------------------------------
  if [ ! -z "${has_sparselizard}" ]; then
  
    if [ ! -e sparselizard-${m}-${c}.sigmay ]; then
      echo "Running Sparselizard m = ${m} c = ${c}"
      cd sparselizard
      ${time} -o ../sparselizard-${m}-${c}.time ./run_sparselizard.sh ${m}-${c} > ../sparselizard-${m}-${c}.sigmay
      cd ..
    fi
    
    if [ -e sparselizard-${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' sparselizard-${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> sparselizard-${m}.dat
        cat sparselizard-${m}-${c}.sigmay | tr "\n" "\t" >> sparselizard-${m}.dat
        cat sparselizard-${m}-${c}.time   | tr "\n" "\t" >> sparselizard-${m}.dat
        echo >> sparselizard-${m}.dat
      fi  
    fi  
  fi
  

  
  # ---- Code Aster ----------------------------------
  if [ ! -z "${has_aster}" ]; then
    if [ ! -e le10_2nd-${m}-${c}.unv ]; then
      gmsh -3 le10_2nd-${m}-${c}.msh -o le10_2nd-${m}-${c}.unv || exit 1
    fi
    
    if [ ! -e aster-${m}-${c}.sigmay ]; then
      echo "running Aster c = ${c}"
      ln -sf le10_2nd-${m}-${c}.unv le10_2nd.unv
      ${time} -o aster-${m}-${c}.time as_run le10.export
      grep "degrés de liberté:" message  | awk '{printf("%g\t", $7)}' > aster-${m}-${c}.sigmay
      grep "2.00000000000000E+03  0.00000000000000E+00  3.00000000000000E+02" DD.txt | awk '{print $5}' >> aster-${m}-${c}.sigmay
    fi

    if [ -e aster-${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' aster-${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> aster-${m}.dat
        cat aster-${m}-${c}.sigmay | tr "\n" "\t" >> aster-${m}.dat
        cat aster-${m}-${c}.time   | tr "\n" "\t" >> aster-${m}.dat
        echo >> aster-${m}.dat
      fi  
    fi
  fi

  # ---- Reflex MUMPS ----------------------------------
  if [ ! -z "${has_reflex}" ]; then
    if [ ! -e reflex_mumps-${m}-${c}.sigmay ]; then
      echo "running Reflex MUMPS c = ${c}"
      ${time} -o reflex_mumps-${m}-${c}.time reflexCLI -i le10.json -s "c=${m}-${c}" -s "x=mumps"
      grep "degrees of freedom" output/le10.log | awk '{printf("%g\t", $3)}' > reflex_mumps-${m}-${c}.sigmay
      jq .outputs.kpi_data[0].symm_tensor.data[0].yy output/le10_result_kpi.json >> reflex_mumps-${m}-${c}.sigmay
    fi
    
    if [ -e reflex_mumps-${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' reflex_mumps-${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> reflex_mumps-${m}.dat
        cat reflex_mumps-${m}-${c}.sigmay | tr "\n" "\t" >> reflex_mumps-${m}.dat
        cat reflex_mumps-${m}-${c}.time   | tr "\n" "\t" >> reflex_mumps-${m}.dat
        echo >> reflex_mumps-${m}.dat
      fi
    fi
  fi
  
  # ---- Reflex GAMG ----------------------------------
  if [ ! -z "${has_reflex}" ]; then
    if [ ! -e reflex_gamg-${m}-${c}.sigmay ]; then
      echo "running Reflex GAMG c = ${c}"
      ${time} -o reflex_gamg-${m}-${c}.time reflexCLI -i le10.json -s "c=${m}-${c}" -s "x=gamg"
      grep "degrees of freedom" output/le10.log | awk '{printf("%g\t", $3)}' > reflex_gamg-${m}-${c}.sigmay
      jq .outputs.kpi_data[0].symm_tensor.data[0].yy output/le10_result_kpi.json >> reflex_gamg-${m}-${c}.sigmay
    fi
    
    if [ -e reflex_gamg-${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' reflex_gamg-${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> reflex_gamg-${m}.dat
        cat reflex_gamg-${m}-${c}.sigmay | tr "\n" "\t" >> reflex_gamg-${m}.dat
        cat reflex_gamg-${m}-${c}.time   | tr "\n" "\t" >> reflex_gamg-${m}.dat
        echo >> reflex_gamg-${m}.dat
      fi
    fi
  fi
  
  # ---- Reflex HYPRE ----------------------------------
  if [ ! -z "${has_reflex}" ]; then
    if [ ! -e reflex_hypre-${m}-${c}.sigmay ]; then
      echo "running Reflex HYPRE c = ${c}"
      ${time} -o reflex_hypre-${m}-${c}.time reflexCLI -i le10.json -s "c=${m}-${c}" -s "x=hypre"
      grep "degrees of freedom" output/le10.log | awk '{printf("%g\t", $3)}' > reflex_hypre-${m}-${c}.sigmay
      jq .outputs.kpi_data[0].symm_tensor.data[0].yy output/le10_result_kpi.json >> reflex_hypre-${m}-${c}.sigmay
    fi
    
    if [ -e reflex_hypre-${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' reflex_hypre-${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> reflex_hypre-${m}.dat
        cat reflex_hypre-${m}-${c}.sigmay | tr "\n" "\t" >> reflex_hypre-${m}.dat
        cat reflex_hypre-${m}-${c}.time   | tr "\n" "\t" >> reflex_hypre-${m}.dat
        echo >> reflex_hypre-${m}.dat
      fi
    fi
  fi
  
 done
done
