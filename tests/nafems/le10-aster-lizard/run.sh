#!/bin/bash

if [[ -z "${1}" ]] || [[ -z "${2}" ]] || [[ -z "${3}" ]]; then 
  if [ "x${1}" != "x--check" ]; then
    echo "usage: $0 { tet | hex } min_clscale n_steps"
    exit 0  
  fi  
fi

m=${1}
min=${2}
steps=${3}

# check for needed tools
for i in grep awk gawk gmsh jq /usr/bin/time; do
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
  rm -f feenox-${m}*.dat
  rm -f feenox_mumps-${m}*.dat
  $(feenox -V | grep mumps > /dev/null)
  if [ $? -eq 0 ]; then
    has_feenox_mumps="yes"
    rm -f feenox_mumps-*.dat
  fi
fi

# --- Sparselizard ----------------------gi
if [ -e "sparselizard/sparselizard" ]; then
  has_sparselizard="yes"
  rm -f sparselizard-${m}*.dat
fi

# --- Code Aster ----------------------
if [ ! -z "$(which as_run)" ]; then
  has_aster="yes"
  rm -f aster-${m}*.dat
fi

# --- CalculiX ----------------------
if [ ! -z "$(which ccx)" ]; then
  has_calculix="yes"
  rm -f calculix-${m}*.dat
fi


# --- Reflex ----------------------
if [ ! -z "$(which reflexCLI)" ]; then
  has_reflex="yes"
  rm -f reflex_gamg-${m}*.dat
  rm -f reflex_mumps-${m}*.dat
  rm -f reflex_hypre-${m}*.dat
fi

has_feenox=""
has_feenox_mumps=""
has_sparselizard=""
has_reflex=""

# TODO: --check
if [ "x${1}" == "x--check" ]; then
  echo "FeenoX GAMG:  ${has_feenox}"
  echo "FeenoX MUMPS: ${has_feenox_mumps}"
  echo "Sparselizard: ${has_sparselizard}"
  echo "Code Aster:   ${has_aster}"
  echo "CalculiX:     ${has_calculix}"
  exit 0
fi


for c in $(feenox steps.fee ${min} ${steps}); do

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
      sed s/_m_/${m}-${c}/ le10.export > le10-${m}-${c}.export
      ${time} -o aster-${m}-${c}.time as_run le10-${m}-${c}.export
      grep "degrés de liberté:" message-${m}-${c}  | awk '{printf("%g\t", $7)}' > aster-${m}-${c}.sigmay
      grep "2.00000000000000E+03  0.00000000000000E+00  3.00000000000000E+02" DD-${m}-${c}.txt | awk '{print $5}' >> aster-${m}-${c}.sigmay
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
  
  
# --- CalculiX ----------------------
  if [ ! -z "${has_calculix}" ]; then
    if [ ! -e le10_2nd-gon-${m}-${c}.unv ]; then
      gmsh-dev -3 le10_2nd-${m}-${c}.msh -setnumber Mesh.SaveGroupsOfElements 1 -setnumber Mesh.SaveGroupsOfNodes 1 -o le10_2nd-gon-${m}-${c}.unv || exit 1
#       gmsh -3 le10-${m}.geo -clscale ${c} -setnumber Mesh.SaveGroupsOfNodes 1 -o le10_2nd-gon-${m}-${c}.unv || exit 1
      ./unical1 le10_2nd-gon-${m}-${c}.unv le10_2nd-${m}-${c}.inp
    fi
    
    if [ ! -e calculix-${m}-${c}.sigmay ]; then
      echo "running CalculiX c = ${c}"
      sed s/xxx/${m}-${c}/ le10.inp > le10-${m}-${c}.inp
      ${time} -o calculix-${m}-${c}.time ccx -i le10-${m}-${c} | tee calculix-${m}-${c}.txt
      grep -C 1 "number of equations" calculix-${m}-${c}.txt | tail -n 1 | awk '{printf("%d\t", $1)}' > calculix-${m}-${c}.sigmay
      gawk -f calculix-stress-at-node.awk le10-${m}-${c}.frd >> calculix-${m}-${c}.sigmay
    fi 
    
    if [ -e calculix-${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' calculix-${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> calculix-${m}.dat
        cat calculix-${m}-${c}.sigmay | tr "\n" "\t" >> calculix-${m}.dat
        cat calculix-${m}-${c}.time   | tr "\n" "\t" >> calculix-${m}.dat
        echo >> calculix-${m}.dat
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
