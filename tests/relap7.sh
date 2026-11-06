#!/bin/sh
for i in . tests; do
  if [ -e ${i}/functions.sh ]; then
    . ${i}/functions.sh 
  fi
done
if [ -z "${functions_found}" ]; then
  echo "could not find functions.sh"
   exit 1
fi

checkpde thermal_hydraulics
checkgmsh

gmsh -v 0 -1 ${dir}/relap7-pipe.geo || exit $?
answerzero relap7-pipe.fee 1e-4
exitifwrong $?

# Independent IF97 + TRACE finite-volume calculation: T_out = 563.06051189 K.
answer1 relap7-heated-pipe.fee 0 "559.801 563.061 25373.5 0.293423"
exitifwrong $?
answer1 relap7-heated-pipe.fee 10 "559.801 563.061 25373.5 0.293423"
exitifwrong $?

gmsh -v 0 -1 ${dir}/relap7-two-pipes.geo || exit $?
gmsh -v 0 -1 -setnumber ReverseFlow 1 ${dir}/relap7-two-pipes.geo -o ${dir}/relap7-two-pipes-reverse.msh || exit $?
gmsh -v 0 -1 -setnumber Orientation 1 ${dir}/relap7-two-pipes.geo -o ${dir}/relap7-two-pipes-oriented.msh || exit $?
checkjunction() {
  echo "Checking two-pipe balance: $*"
  answer=$(${feenox} ${dir}/relap7-two-pipes-verify.fee "$@") || exit $?
  result=$(${feenox} ${dir}/cmp-zero.fee "(${answer})" 1e-7) || exit $?
  if [ "${result}" != 1 ]; then
    echo "expected zero balance error, got ${answer}"
    exit 1
  fi
}
for mesh in relap7-two-pipes.msh relap7-two-pipes-reverse.msh relap7-two-pipes-oriented.msh; do
  checkjunction "${mesh}"
  checkjunction "${mesh}" "K=0 A_ref=1" 0 0 0 0 1
  checkjunction "${mesh}" "A_ref=1" 0 0 0 0 1
  checkjunction "${mesh}" "K_pipe1=1 A_ref=1" 0 0 1 0 1
  checkjunction "${mesh}" "K=1" 0 0 1 1 0
  checkjunction "${mesh}" "K=9 K_pipe1=0 K_pipe2=2 A_ref=0.5" 0 0 0 2 0.5
done
for loss in 0 1; do
  answerzero1 thermal-hydraulics-junction-equal.fee ${loss} 1e-5
  exitifwrong $?
done
checkjunction relap7-two-pipes.msh "K=if(t<0.2,0,1) A_ref=if(t<0.2,2,1)" 10
answerzero relap7-two-pipes-startup-verify.fee 1e-6
exitifwrong $?
gmsh -v 0 -1 ${dir}/thermal-hydraulics-tee.geo || exit $?
answerzero thermal-hydraulics-junction-tee.fee 1e-6
exitifwrong $?
for properties in "K=-1" "A_ref=0" "A_ref=-1" "K_pipe1=-1" "K_missing=1"; do
  echo "Rejecting invalid junction properties: ${properties}"
  if ${feenox} ${dir}/thermal-hydraulics-junction-invalid.fee connection "${properties}"; then
    echo "expected an error"
    exit 1
  fi
done
for target in pipe1 inlet; do
  echo "Rejecting junction properties on ${target}"
  if ${feenox} ${dir}/thermal-hydraulics-junction-invalid.fee "${target}" "K=1"; then
    echo "expected an error"
    exit 1
  fi
done
if ${feenox} ${dir}/thermal-hydraulics-junction-invalid.fee connection "K=1" "BC connection p=1e5"; then
  echo "expected an error for a pressure BC at an internal junction"
  exit 1
fi

gmsh -v 0 -1 ${dir}/relap7-core-channel.geo || exit $?
for declaration in "HEAT_STRUCTURE" "HEAT_STRUCTURE PIPE channel" "HEAT_STRUCTURE FUEL fuel" "HEAT_STRUCTURE PIPE channel FUEL fuel extra" "HEAT_STRUCTURE PIPE channel FUEL" "HEAT_STRUCTURE PIPE channel FUEL fuel GAP" "HEAT_STRUCTURE PIPE channel FUEL fuel CLAD" "HEAT_STRUCTURE PIPE channel FUEL fuel PIPE other" "HEAT_STRUCTURE PIPE channel FUEL fuel FUEL other" "HEAT_STRUCTURE PIPE channel FUEL fuel GAP gap GAP other" "HEAT_STRUCTURE PIPE channel FUEL fuel CLAD clad CLAD other"; do
  echo "Rejecting invalid heat-structure declaration: ${declaration}"
  if ${feenox} --check ${dir}/relap7-core-parser.fee "PROBLEM thermal_hydraulics" "${declaration}" "#"; then
    echo "expected an error"
    exit 1
  fi
done
for flags in "AXIAL_CONDUCTION NO_AXIAL_CONDUCTION" "NO_AXIAL_CONDUCTION AXIAL_CONDUCTION" "AXIAL_CONDUCTION AXIAL_CONDUCTION" "NO_AXIAL_CONDUCTION NO_AXIAL_CONDUCTION"; do
  echo "Rejecting duplicate or conflicting conduction flags: ${flags}"
  if ${feenox} --check ${dir}/relap7-core-parser.fee "PROBLEM thermal_hydraulics" "HEAT_STRUCTURE PIPE channel FUEL fuel ${flags}" "#"; then
    echo "expected an error"
    exit 1
  fi
done
for keyword in PIPE FUEL GAP CLAD; do
  declaration="HEAT_STRUCTURE PIPE channel FUEL fuel ${keyword}"
  if [ "${keyword}" = PIPE ]; then
    declaration="HEAT_STRUCTURE FUEL fuel PIPE"
  elif [ "${keyword}" = FUEL ]; then
    declaration="HEAT_STRUCTURE PIPE channel FUEL"
  fi
  if diagnostic=$(${feenox} --check ${dir}/relap7-core-parser.fee "PROBLEM thermal_hydraulics" "${declaration}" "#" 2>&1); then
    echo "expected an error for missing ${keyword} parameter"
    exit 1
  fi
  echo "${diagnostic}" | grep -q "keyword '${keyword}' needs a parameter" || exit 1
done
for keyword in PIPE FUEL; do
  declaration="HEAT_STRUCTURE PIPE channel"
  if [ "${keyword}" = PIPE ]; then
    declaration="HEAT_STRUCTURE FUEL fuel"
  fi
  if diagnostic=$(${feenox} --check ${dir}/relap7-core-parser.fee "PROBLEM thermal_hydraulics" "${declaration}" "#" 2>&1); then
    echo "expected an error for missing ${keyword} keyword"
    exit 1
  fi
  echo "${diagnostic}" | grep -q "requires keyword '${keyword}'" || exit 1
done
for problem in "dummy=0" "PROBLEM thermal" "PROBLEM thermal_hydraulics HEAT_STRUCTURE channel fuel gap clad"; do
  echo "Rejecting invalid heat-structure problem: ${problem}"
  if ${feenox} --check ${dir}/relap7-core-parser.fee "${problem}" "HEAT_STRUCTURE PIPE channel FUEL fuel GAP gap CLAD clad" "#"; then
    echo "expected an error"
    exit 1
  fi
done
if ${feenox} --check ${dir}/relap7-core-parser.fee "PROBLEM thermal_hydraulics" "HEAT_STRUCTURE PIPE channel FUEL fuel GAP gap CLAD clad" "HEAT_STRUCTURE FUEL fuel PIPE channel"; then
  echo "expected an error for duplicate HEAT_STRUCTURE"
  exit 1
fi
${feenox} --check ${dir}/relap7-core-parser.fee "PROBLEM thermal_hydraulics" "HEAT_STRUCTURE PIPE channel FUEL fuel" "HEAT_STRUCTURE FUEL fuel PIPE other" || exit $?
${feenox} --check ${dir}/relap7-core-parser.fee "PROBLEM thermal_hydraulics" "HEAT_STRUCTURE PIPE channel FUEL fuel GAP gap CLAD clad" "WRITE_RESULTS FORMAT vtu all" || exit $?
gmsh -v 0 -1 ${dir}/thermal-hydraulics-structures.geo || exit $?
answerzero thermal-hydraulics-junction-series.fee 1e-5
exitifwrong $?
for axial in 0 1; do
  if [ ${axial} = 0 ]; then
    conduction=NO_AXIAL_CONDUCTION
  else
    conduction=AXIAL_CONDUCTION
  fi
  answerzero1 thermal-hydraulics-structures.fee ${conduction} 0.01
  exitifwrong $?
  answerzero1 thermal-hydraulics-structures-energy.fee ${conduction} 1e-6
  exitifwrong $?
  answerzero2 thermal-hydraulics-structures-axial.fee ${conduction} ${axial} 1e-4
  exitifwrong $?
  answerzero1 relap7-core-verify.fee ${conduction} 2e-3
  exitifwrong $?
  answerzero1 relap7-core-energy.fee ${conduction} 1e-6
  exitifwrong $?
  answerzero2 relap7-core-axial.fee ${conduction} ${axial} 1e-5
  exitifwrong $?
  answerzero2 relap7-core-verify.fee ${conduction} 1 2e-3
  exitifwrong $?
  if [ ${axial} = 0 ]; then
    expected="604.544 1404.87 763.445 617.127 0.293606"
  else
    expected="604.544 1404.87 763.444 617.127 0.293606"
  fi
  answer2 relap7-core-startup.fee ${conduction} 50 "${expected}"
  exitifwrong $?
done

${feenox} ${dir}/thermal-hydraulics-structures-output.fee || exit $?
for group in first second third; do
  for field in T_fuel_center T_fuel_surface T_clad_surface; do
    grep -q "Name=\"${field}_${group}\"" thermal-hydraulics-structures-all.vtu || exit 1
  done
done
for field in T_fuel_center_first T_fuel_surface_second T_clad_surface_third; do
  grep -q "Name=\"${field}\"" thermal-hydraulics-structures-selected.vtu || exit 1
done
rm -f thermal-hydraulics-structures-all.vtu thermal-hydraulics-structures-selected.vtu

solid="k=2.5 rho_s=10320 cp=300"
geometry="fuel_radius=0.004096 gap_width=0.0001 clad_width=0.000552 fuel_cells=10 gap_cells=1 clad_cells=2"
for properties in "h_wall=0 hs_power=77337" "hs_power=77337" "h_wall=53300 hs_power=-1" "h_wall=53300 hs_power=z" "h_wall=53300 hs_power=77337 T_wall=565" "h_wall=53300 hs_power=77337 q_wall=0" "h_wall=53300 hs_power=77337 P_h=0"; do
  echo "Rejecting invalid core properties: ${properties}"
  if ${feenox} ${dir}/relap7-core-invalid.fee "${solid}" "${geometry} ${properties}" "dummy=0"; then
    echo "expected an error"
    exit 1
  fi
done
for material in "k=0 rho_s=10320 cp=300" "k=2.5 cp=300" "k=2.5 rho_s=10320 cp=300+t"; do
  echo "Rejecting invalid solid material: ${material}"
  if ${feenox} ${dir}/relap7-core-invalid.fee "${material}" "${geometry} h_wall=53300 hs_power=77337" "dummy=0"; then
    echo "expected an error"
    exit 1
  fi
done
for initialization in "T_solid_0(r,x)=559.15" "T_solid_0(r,x,y,z)=-1" "mdot_0(x,y,z)=0"; do
  echo "Rejecting invalid core initialization: ${initialization}"
  if ${feenox} ${dir}/relap7-core-invalid.fee "${solid}" "${geometry} h_wall=53300 hs_power=77337" "${initialization}"; then
    echo "expected an error"
    exit 1
  fi
done
for fuel_cells in 0 1.5 10001 "10+t" "10+z"; do
  echo "Rejecting invalid radial cell count: ${fuel_cells}"
  if ${feenox} ${dir}/relap7-core-invalid.fee "${solid}" "fuel_radius=0.004096 gap_width=0.0001 clad_width=0.000552 fuel_cells=${fuel_cells} gap_cells=1 clad_cells=2 h_wall=53300 hs_power=77337" "dummy=0"; then
    echo "expected an error"
    exit 1
  fi
done
