#!/bin/bash

# if you want to disable any particular code, search below for has_feenox=""
# and uncomment the proper lines

# TODO: use getopt
if [[ -z "${1}" ]] || [[ -z "${2}" ]] || [[ -z "${3}" ]]; then 
  if [ "x${1}" != "x--check" ]; then
    echo "usage: $0 --check"
    echo "usage: $0 { tet | hex } c_min n_steps"
    exit 0  
  fi  
fi

m=${1}
min=${2}
steps=${3}

# check for needed tools for global usage
# TODO: make sure Gmsh is new enough for calculix
for i in /usr/bin/time gmsh feenox grep cat; do
 if [ -z "$(which $i)" ]; then
  echo "error: ${i} needed for global usage but not installed"
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
  rm -f feenox_gamg_${m}*.dat
  $(feenox -V | grep mumps > /dev/null)
  if [ $? -eq 0 ]; then
    has_feenox_mumps="yes"
    rm -f feenox_mumps_${m}*.dat
  fi
fi

# --- Sparselizard ----------------------gi
if [ -e "sparselizard/sparselizard" ]; then
  has_sparselizard="yes"
  rm -f sparselizard_mumps_${m}*.dat
fi

# --- Code Aster ----------------------
if [ ! -z "$(which as_run)" ]; then

  for i in sed awk; do
    if [ -z "$(which $i)" ]; then
      echo "error: ${i} needed for Code Aster but not installed"
      exit 1
    fi
  done

  has_aster="yes"
  rm -f aster_default_${m}*.dat
fi

# --- CalculiX ----------------------
if [ ! -z "$(which ccx)" ]; then

  for i in cc gawk; do
    if [ -z "$(which $i)" ]; then
      echo "error: ${i} needed for CalculiX but not installed"
      exit 1
    fi
  done

  has_calculix="yes"
  cc unical1.c -o unical1
  rm -f calculix_${m}*.dat
fi


# --- Reflex ----------------------
if [ ! -z "$(which reflexCLI)" ]; then

  for i in jq; do
    if [ -z "$(which $i)" ]; then
      echo "error: ${i} needed for RefleX but not installed"
      exit 1
    fi
  done

  has_reflex="yes"
  rm -f reflex_gamg_${m}*.dat
  rm -f reflex_mumps_${m}*.dat
  rm -f reflex_hypre_${m}*.dat
fi

# manually-disabled codes

# has_feenox=""
# has_feenox_mumps=""
# has_sparselizard=""
# has_aster=""
# has_calculix=""
# has_reflex=""

# TODO: --check
if [ "x${1}" == "x--check" ]; then
  echo "FeenoX GAMG:  ${has_feenox}"
  echo "FeenoX MUMPS: ${has_feenox_mumps}"
  echo "Sparselizard: ${has_sparselizard}"
  echo "Code Aster:   ${has_aster}"
  echo "CalculiX:     ${has_calculix}"
  exit 0
fi


# create a markdown table with some data about the architecture
cat << EOF > arch-${m}.md

|         |        
|---------|------------------------------------------------------------------------------------------------
| Date    |  $(date)
| Host    |  $(uname -a) 
| CPU     |  $(cat /proc/cpuinfo | grep "model name" | head -n1 | cut -d: -f2) 
| Number  |  $(cat /proc/cpuinfo  | grep processor | wc -l)
| Memory  |  $(cat /proc/meminfo  | head -n1 | cut -d: -f2)
  
EOF


for c in $(feenox steps.fee ${min} ${steps}); do

  if [ ! -e le10-${m}-${c}.msh ]; then
    gmsh -3 le10-${m}.geo -clscale ${c} -o le10-${m}-${c}.msh || exit 1
    gmsh -3 le10-${m}-${c}.msh -setnumber Mesh.SecondOrderIncomplete 1 -order 2 -o le10_2nd-${m}-${c}.msh || exit 1
  fi

  # ---- FeenoX (GAMG) -----------------------------------------
  if [ ! -z "${has_feenox}" ]; then
    if [ ! -e feenox_gamg_${m}-${c}.sigmay ]; then
      echo "running FeenoX GAMG m = ${m} c = ${c}"
      ${time} -o feenox_gamg_${m}-${c}.time feenox le10.fee ${m}-${c} > feenox_gamg_${m}-${c}.sigmay
    fi
    
    if [ -e feenox_gamg_${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' feenox_gamg_${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> feenox_gamg_${m}.dat
        cat feenox_gamg_${m}-${c}.sigmay | tr "\n" "\t" >> feenox_gamg_${m}.dat
        cat feenox_gamg_${m}-${c}.time   | tr "\n" "\t" >> feenox_gamg_${m}.dat
        echo >> feenox_gamg_${m}.dat
      fi  
    fi
  fi  

  # ---- FeenoX (MUMPS) -----------------------------------------
  if [ ! -z "${has_feenox_mumps}" ]; then
    if [ ! -e feenox_mumps_${m}-${c}.sigmay ]; then
      echo "running FeenoX MUMPS c = ${c}"
      ${time} -o feenox_mumps_${m}-${c}.time feenox le10.fee ${m}-${c} --mumps > feenox_mumps_${m}-${c}.sigmay
    fi

    if [ -e feenox_mumps_${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' feenox_mumps_${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> feenox_mumps_${m}.dat
        cat feenox_mumps_${m}-${c}.sigmay | tr "\n" "\t" >> feenox_mumps_${m}.dat
        cat feenox_mumps_${m}-${c}.time   | tr "\n" "\t" >> feenox_mumps_${m}.dat
        echo >> feenox_mumps_${m}.dat
      fi  
    fi  
  fi
  
  # ---- Sparselizard ----------------------------------
  if [ ! -z "${has_sparselizard}" ]; then
  
    if [ ! -e sparselizard_mumps_${m}-${c}.sigmay ]; then
      echo "Running Sparselizard m = ${m} c = ${c}"
      cd sparselizard
      ${time} -o ../sparselizard_mumps_${m}-${c}.time ./run_sparselizard.sh ${m}-${c} > ../sparselizard_mumps_${m}-${c}.sigmay
      cd ..
    fi
    
    if [ -e sparselizard_mumps_${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' sparselizard_mumps_${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> sparselizard_mumps_${m}.dat
        cat sparselizard_mumps_${m}-${c}.sigmay | tr "\n" "\t" >> sparselizard_mumps_${m}.dat
        cat sparselizard_mumps_${m}-${c}.time   | tr "\n" "\t" >> sparselizard_mumps_${m}.dat
        echo >> sparselizard_mumps_${m}.dat
      fi  
    fi  
  fi
  

  
  # ---- Code Aster ----------------------------------
  if [ ! -z "${has_aster}" ]; then
    if [ ! -e le10_2nd-${m}-${c}.unv ]; then
      gmsh -3 le10_2nd-${m}-${c}.msh -o le10_2nd-${m}-${c}.unv || exit 1
    fi
    
    # default
    if [ ! -e aster_default_${m}-${c}.sigmay ]; then
      echo "running Aster c = ${c}"
      sed s/_m_/${m}-${c}/ le10.export | sed s/_s_/default/ > le10_default-${m}-${c}.export
      ${time} -o aster_default_${m}-${c}.time as_run le10_default-${m}-${c}.export
      grep "degrés de liberté:" message_default-${m}-${c} | head -n1 | awk '{printf("%g\t", $7)}' > aster_default_${m}-${c}.sigmay
      grep "2.00000000000000E+03  0.00000000000000E+00  3.00000000000000E+02" DD-default-${m}-${c}.txt | awk '{print $5}' >> aster_default_${m}-${c}.sigmay
    fi

    if [ -e aster_default_${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' aster_default_${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> aster_default_${m}.dat
        cat aster_default_${m}-${c}.sigmay | tr "\n" "\t" >> aster_default_${m}.dat
        cat aster_default_${m}-${c}.time   | tr "\n" "\t" >> aster_default_${m}.dat
        echo >> aster_default_${m}.dat
      fi  
    fi
    
    # cholesky (subintegrated)
    if [ ! -e aster_cholesky_${m}-${c}.sigmay ]; then
      echo "running Aster c = ${c}"
      sed s/_m_/${m}-${c}/ le10.export | sed s/_s_/cholesky/ > le10_cholesky-${m}-${c}.export
      ${time} -o aster_cholesky_${m}-${c}.time as_run le10_cholesky-${m}-${c}.export
      grep "degrés de liberté:" message_cholesky-${m}-${c} | head -n1 | awk '{printf("%g\t", $7)}' > aster_cholesky_${m}-${c}.sigmay
      grep "2.00000000000000E+03  0.00000000000000E+00  3.00000000000000E+02" DD-cholesky-${m}-${c}.txt | awk '{print $5}' >> aster_cholesky_${m}-${c}.sigmay
    fi

    if [ -e aster_cholesky_${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' aster_cholesky_${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> aster_cholesky_${m}.dat
        cat aster_cholesky_${m}-${c}.sigmay | tr "\n" "\t" >> aster_cholesky_${m}.dat
        cat aster_cholesky_${m}-${c}.time   | tr "\n" "\t" >> aster_cholesky_${m}.dat
        echo >> aster_cholesky_${m}.dat
      fi  
    fi
    
    # mumps
    if [ ! -e aster_mumps_${m}-${c}.sigmay ]; then
      echo "running Aster c = ${c}"
      sed s/_m_/${m}-${c}/ le10.export | sed s/_s_/mumps/ > le10_mumps-${m}-${c}.export
      ${time} -o aster_mumps_${m}-${c}.time as_run le10_mumps-${m}-${c}.export
      grep "degrés de liberté:" message_mumps-${m}-${c}  | head -n1 | awk '{printf("%g\t", $7)}' > aster_mumps_${m}-${c}.sigmay
      grep "2.00000000000000E+03  0.00000000000000E+00  3.00000000000000E+02" DD-mumps-${m}-${c}.txt | awk '{print $5}' >> aster_mumps_${m}-${c}.sigmay
    fi

    if [ -e aster_mumps_${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' aster_mumps_${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> aster_mumps_${m}.dat
        cat aster_mumps_${m}-${c}.sigmay | tr "\n" "\t" >> aster_mumps_${m}.dat
        cat aster_mumps_${m}-${c}.time   | tr "\n" "\t" >> aster_mumps_${m}.dat
        echo >> aster_mumps_${m}.dat
      fi  
    fi
    
    
  fi
  
  
  # --- CalculiX ----------------------
  if [ ! -z "${has_calculix}" ]; then
    if [ ! -e le10_mesh-${m}-${c}.inp ]; then
      gmsh -3 le10_2nd-${m}-${c}.msh -setnumber Mesh.SaveGroupsOfElements 1 -setnumber Mesh.SaveGroupsOfNodes 1 -o le10_calculix-${m}-${c}.unv || exit 1
      ./unical1 le10_calculix-${m}-${c}.unv le10_mesh-${m}-${c}.inp || exit
    fi
    
    # Spooles
    if [[ -e le10-${m}.inp ]] && [[ -e le10_mesh-${m}-${c}.inp ]]; then
      if [ ! -e calculix_spooles_${m}-${c}.sigmay ]; then
        echo "running CalculiX c = ${c} Spooles"
        sed s/xxx/${c}/ le10-${m}.inp | sed 's/**Static, Solver=Spooles/*Static, Solver=Spooles/' > le10_spooles_${m}-${c}.inp
        ${time} -o calculix_spooles_${m}-${c}.time ccx -i le10_spooles_${m}-${c} | tee calculix_spooles_${m}-${c}.txt
        grep -C 1 "number of equations" calculix_spooles_${m}-${c}.txt | tail -n 1 | awk '{printf("%d\t", $1)}' > calculix_spooles_${m}-${c}.sigmay
        gawk -f frd-stress-at-node.awk le10_spooles_${m}-${c}.frd >> calculix_spooles_${m}-${c}.sigmay
      fi  
    fi 
    
    if [ -e calculix_spooles_${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' calculix_spooles_${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> calculix_spooles_${m}.dat
        cat calculix_spooles_${m}-${c}.sigmay | tr "\n" "\t" >> calculix_spooles_${m}.dat
        cat calculix_spooles_${m}-${c}.time   | tr "\n" "\t" >> calculix_spooles_${m}.dat
        echo >> calculix_spooles_${m}.dat
      fi  
    fi
    
    # Iterative diagonal
    if [[ -e le10-${m}.inp ]] && [[ -e le10_mesh-${m}-${c}.inp ]]; then
      if [ ! -e calculix_diagonal_${m}-${c}.sigmay ]; then
        echo "running CalculiX c = ${c} Iterative diagonal"
        sed s/xxx/${c}/ le10-${m}.inp | sed 's/**Static, Solver=Iterative scaling/*Static, Solver=Iterative scaling/' > le10_diagonal_${m}-${c}.inp
        ${time} -o calculix_diagonal_${m}-${c}.time ccx -i le10_diagonal_${m}-${c} | tee calculix_diagonal_${m}-${c}.txt
        grep -C 1 "number of equations" calculix_diagonal_${m}-${c}.txt | tail -n 1 | awk '{printf("%d\t", $1)}' > calculix_diagonal_${m}-${c}.sigmay
        gawk -f frd-stress-at-node.awk le10_diagonal_${m}-${c}.frd >> calculix_diagonal_${m}-${c}.sigmay
      fi  
    fi 
    
    if [ -e calculix_diagonal_${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' calculix_diagonal_${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> calculix_diagonal_${m}.dat
        cat calculix_diagonal_${m}-${c}.sigmay | tr "\n" "\t" >> calculix_diagonal_${m}.dat
        cat calculix_diagonal_${m}-${c}.time   | tr "\n" "\t" >> calculix_diagonal_${m}.dat
        echo >> calculix_diagonal_${m}.dat
      fi  
    fi

    # Iterative cholesky
    if [[ -e le10-${m}.inp ]] && [[ -e le10_mesh-${m}-${c}.inp ]]; then
      if [ ! -e calculix_cholesky_${m}-${c}.sigmay ]; then
        echo "running CalculiX c = ${c} Iterative cholesky"
        sed s/xxx/${c}/ le10-${m}.inp | sed 's/**Static, Solver=Iterative Cholesky/*Static, Solver=Iterative Cholesky/' > le10_cholesky_${m}-${c}.inp
        ${time} -o calculix_cholesky_${m}-${c}.time ccx -i le10_cholesky_${m}-${c} | tee calculix_cholesky_${m}-${c}.txt
        grep -C 1 "number of equations" calculix_cholesky_${m}-${c}.txt | tail -n 1 | awk '{printf("%d\t", $1)}' > calculix_cholesky_${m}-${c}.sigmay
        gawk -f frd-stress-at-node.awk le10_cholesky_${m}-${c}.frd >> calculix_cholesky_${m}-${c}.sigmay
      fi  
    fi 
    
    if [ -e calculix_cholesky_${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' calculix_cholesky_${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> calculix_cholesky_${m}.dat
        cat calculix_cholesky_${m}-${c}.sigmay | tr "\n" "\t" >> calculix_cholesky_${m}.dat
        cat calculix_cholesky_${m}-${c}.time   | tr "\n" "\t" >> calculix_cholesky_${m}.dat
        echo >> calculix_cholesky_${m}.dat
      fi  
    fi
    
  fi
  

  # ---- Reflex MUMPS ----------------------------------
  if [ ! -z "${has_reflex}" ]; then
    if [ ! -e reflex_mumps_${m}-${c}.sigmay ]; then
      echo "running Reflex MUMPS c = ${c}"
      ${time} -o reflex_mumps_${m}-${c}.time reflexCLI -i le10.json -s "c=${m}-${c}" -s "x=mumps"
      grep "degrees of freedom" output/le10.log | awk '{printf("%g\t", $3)}' > reflex_mumps_${m}-${c}.sigmay
      jq .outputs.kpi_data[0].symm_tensor.data[0].yy output/le10_result_kpi.json >> reflex_mumps_${m}-${c}.sigmay
    fi
    
    if [ -e reflex_mumps_${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' reflex_mumps_${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> reflex_mumps_${m}.dat
        cat reflex_mumps_${m}-${c}.sigmay | tr "\n" "\t" >> reflex_mumps_${m}.dat
        cat reflex_mumps_${m}-${c}.time   | tr "\n" "\t" >> reflex_mumps_${m}.dat
        echo >> reflex_mumps_${m}.dat
      fi
    fi
  fi
  
  # ---- Reflex GAMG ----------------------------------
  if [ ! -z "${has_reflex}" ]; then
    if [ ! -e reflex_gamg_${m}-${c}.sigmay ]; then
      echo "running Reflex GAMG c = ${c}"
      ${time} -o reflex_gamg_${m}-${c}.time reflexCLI -i le10.json -s "c=${m}-${c}" -s "x=gamg"
      grep "degrees of freedom" output/le10.log | awk '{printf("%g\t", $3)}' > reflex_gamg_${m}-${c}.sigmay
      jq .outputs.kpi_data[0].symm_tensor.data[0].yy output/le10_result_kpi.json >> reflex_gamg_${m}-${c}.sigmay
    fi
    
    if [ -e reflex_gamg_${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' reflex_gamg_${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> reflex_gamg_${m}.dat
        cat reflex_gamg_${m}-${c}.sigmay | tr "\n" "\t" >> reflex_gamg_${m}.dat
        cat reflex_gamg_${m}-${c}.time   | tr "\n" "\t" >> reflex_gamg_${m}.dat
        echo >> reflex_gamg_${m}.dat
      fi
    fi
  fi
  
  # ---- Reflex HYPRE ----------------------------------
  if [ ! -z "${has_reflex}" ]; then
    if [ ! -e reflex_hypre_${m}-${c}.sigmay ]; then
      echo "running Reflex HYPRE c = ${c}"
      ${time} -o reflex_hypre_${m}-${c}.time reflexCLI -i le10.json -s "c=${m}-${c}" -s "x=hypre"
      grep "degrees of freedom" output/le10.log | awk '{printf("%g\t", $3)}' > reflex_hypre_${m}-${c}.sigmay
      jq .outputs.kpi_data[0].symm_tensor.data[0].yy output/le10_result_kpi.json >> reflex_hypre_${m}-${c}.sigmay
    fi
    
    if [ -e reflex_hypre_${m}-${c}.sigmay ]; then
      grep 'terminated\|exited\\nan' reflex_hypre_${m}-${c}.*
      if [ $? -ne 0 ]; then
        echo -ne "${c}\t" >> reflex_hypre_${m}.dat
        cat reflex_hypre_${m}-${c}.sigmay | tr "\n" "\t" >> reflex_hypre_${m}.dat
        cat reflex_hypre_${m}-${c}.time   | tr "\n" "\t" >> reflex_hypre_${m}.dat
        echo >> reflex_hypre_${m}.dat
      fi
    fi
  fi
  
done
