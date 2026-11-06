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

# RELAP-7 Example 7: two heated inflows mixing into three heated outflows.
gmsh -v 0 -1 ${dir}/relap7-five-pipes.geo || exit $?
answerzero relap7-five-pipes-verify.fee 1e-6
exitifwrong $?
answerzero relap7-five-pipes-startup-verify.fee 1e-6
exitifwrong $?

# RELAP-7 Example 8: prescribed-head pump and an initially stagnant loop.
gmsh -v 0 -1 ${dir}/relap7-pump-loop.geo || exit $?
gmsh -v 0 -1 -setnumber Orientation 1 ${dir}/relap7-pump-loop.geo -o ${dir}/relap7-pump-loop-oriented.msh || exit $?
checkpump() {
  input=$1
  shift
  echo "Checking pump: ${input} $*"
  answer=$(${feenox} ${dir}/${input} "$@") || exit $?
  result=$(${feenox} ${dir}/cmp-zero.fee "(${answer})" 1e-6) || exit $?
  test "${result}" = 1 || { echo "pump balance error: ${answer}"; exit 1; }
}
answerzero relap7-pump-loop-verify.fee 1e-6
exitifwrong $?
checkpump relap7-pump-loop-verify.fee relap7-pump-loop-oriented.msh 0 "(0-1)"
gmsh -v 0 -1 ${dir}/thermal-hydraulics-pump.geo || exit $?
checkpump thermal-hydraulics-pump.fee 1 1000 1 thermal-hydraulics-pump.msh
checkpump thermal-hydraulics-pump.fee 1 1000 "if(t<0.2,0,1)" relap7-two-pipes.msh 1 10
${feenox} ${dir}/thermal-hydraulics-point-invalid.fee "PUMP A_REF 2 K_REVERSE 10 20 DIRECTION 1 0 0 HEAD 1 POINT connection" || exit $?
${feenox} ${dir}/thermal-hydraulics-point-invalid.fee "JUNCTION POINT connection" || exit $?
for topology in 1 2; do
  gmsh -v 0 -1 -setnumber Topology "${topology}" ${dir}/thermal-hydraulics-pump.geo -o ${dir}/thermal-hydraulics-pump-invalid.msh || exit $?
  if diagnostic=$(${feenox} ${dir}/thermal-hydraulics-point-invalid.fee "PUMP POINT connection HEAD 1 DIRECTION 1 0 0" "dummy=0" thermal-hydraulics-pump-invalid.msh "${topology}" 2>&1); then
    echo "expected an error for invalid pump topology"
    exit 1
  fi
  if [ "${topology}" = 1 ]; then
    echo "${diagnostic}" | grep -q "needs exactly one physical point" || exit 1
  else
    echo "${diagnostic}" | grep -q "needs exactly two incident pipe elements" || exit 1
  fi
done
for direction in 1 "(0-1)"; do
  for area in 1 2; do
    for head in 0 1; do
      checkpump thermal-hydraulics-pump.fee "${direction}" 1000 "${head}" relap7-two-pipes.msh "${area}"
      checkpump thermal-hydraulics-pump.fee "${direction}" "(0-1000)" "${head}" relap7-two-pipes-reverse.msh "${area}"
    done
  done
done
for declaration in \
  "PUMP POINT connection HEAD 1" \
  "PUMP POINT connection DIRECTION 1 0 0" \
  "PUMP HEAD 1 DIRECTION 1 0 0" \
  "PUMP POINT connection HEAD" \
  "PUMP POINT connection HEAD 1 DIRECTION 1 0" \
  "PUMP POINT connection HEAD 1 DIRECTION 0 0 0" \
  "PUMP POINT connection HEAD 1 DIRECTION 0 1 0" \
  "PUMP POINT connection HEAD -1 DIRECTION 1 0 0" \
  "PUMP POINT connection HEAD 1 DIRECTION 1 0 0 K_REVERSE -1 0" \
  "PUMP POINT connection HEAD 1 DIRECTION 1 0 0 K_REVERSE 1" \
  "PUMP POINT connection HEAD 1 DIRECTION 1 0 0 A_REF 0" \
  "PUMP POINT connection HEAD 1 DIRECTION 1 0 0 HEAD 2" \
  "PUMP POINT inlet HEAD 1 DIRECTION 1 0 0" \
  "PUMP POINT pipe1 HEAD 1 DIRECTION 1 0 0" \
  "PUMP POINT missing HEAD 1 DIRECTION 1 0 0" \
  "JUNCTION POINT connection K=-1" \
  "JUNCTION POINT connection A_ref=0" \
  "JUNCTION POINT connection K_missing=1" \
  "JUNCTION POINT connection K=1 K=2" \
  "JUNCTION POINT connection K=" \
  "JUNCTION POINT connection unknown=1"; do
  echo "Rejecting invalid point component: ${declaration}"
  if ${feenox} ${dir}/thermal-hydraulics-point-invalid.fee "${declaration}"; then
    echo "expected an error"
    exit 1
  fi
done
for conflict in \
  "PUMP POINT connection HEAD 1 DIRECTION 1 0 0" \
  "JUNCTION POINT connection K=1" \
  "MATERIAL connection K=1" \
  "BC connection p=1e5"; do
  if ${feenox} ${dir}/thermal-hydraulics-point-invalid.fee "PUMP POINT connection HEAD 1 DIRECTION 1 0 0" "${conflict}"; then
    echo "expected an error for conflicting point declarations"
    exit 1
  fi
done

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

# RELAP-7 Example 9: two hydraulically separate sides of a counterflow exchanger.
gmsh -v 0 -1 ${dir}/relap7-heat-exchanger.geo || exit $?
gmsh -v 0 -1 -setnumber N 100 ${dir}/relap7-heat-exchanger.geo -o ${dir}/relap7-heat-exchanger-fine.msh || exit $?
gmsh -v 0 -1 -setnumber Orientation 1 ${dir}/relap7-heat-exchanger.geo -o ${dir}/relap7-heat-exchanger-oriented.msh || exit $?
gmsh -v 0 -1 -setnumber SecondaryN 9 ${dir}/relap7-heat-exchanger.geo -o ${dir}/relap7-heat-exchanger-mismatch.msh || exit $?
checkexchanger() {
  input=$1
  shift
  echo "Checking exchanger: ${input} $*"
  answer=$(${feenox} ${dir}/${input} "$@") || exit $?
  result=$(${feenox} ${dir}/cmp-zero.fee "(${answer})" 1e-6) || exit $?
  test "${result}" = 1 || { echo "exchanger balance error: ${answer}"; exit 1; }
}
checkexchanger relap7-heat-exchanger-verify.fee
checkexchanger relap7-heat-exchanger-energy.fee
checkexchanger relap7-heat-exchanger-energy.fee 10 relap7-heat-exchanger-oriented.msh 0 "(0-1)"
checkexchanger thermal-hydraulics-exchanger.fee
checkexchanger thermal-hydraulics-exchanger.fee TUBE "INNER_RADIUS 0.005" 0
checkexchanger thermal-hydraulics-exchanger-tube-energy.fee
checkexchanger thermal-hydraulics-exchanger-multiple.fee
checkexchanger thermal-hydraulics-exchanger-rod.fee
for structure in exchanger second; do
  for field in T_wall_primary T_wall_middle T_wall_secondary; do
    grep -q "Name=\"${field}_${structure}\"" thermal-hydraulics-exchanger.vtu || exit 1
  done
done
rm -f thermal-hydraulics-exchanger.vtu thermal-hydraulics-exchanger-rod.vtu
checkexchanger thermal-hydraulics-stagnation.fee
for properties in \
  "p0=1.005e6" "T0=400" "p0=-1 T0=400" "p0=1.005e6 T0=100" \
  "p0=1.005e6 T0=400 p=1e6" "p0=1.005e6 T0=400 T=400" \
  "p0=1.005e6 T0=400 rho=900" "p0=1.005e6 T0=400 v=1" \
  "p0=1.005e6 T0=400 mdot=0.1"; do
  if ${feenox} ${dir}/thermal-hydraulics-stagnation.fee "${properties}"; then
    echo "expected an error for invalid stagnation BC: ${properties}"
    exit 1
  fi
done
if ${feenox} ${dir}/thermal-hydraulics-stagnation.fee "p0=1.005e6 T0=400" connection; then
  echo "expected an error for an internal stagnation BC"
  exit 1
fi
for geometry in \
  "TYPE PLATE THICKNESS 0.01 CELLS 2" \
  "TYPE TUBE THICKNESS 0.01 CELLS 2" \
  "TYPE unknown THICKNESS 0.01 CELLS 2 DEPTH 1" \
  "TYPE PLATE THICKNESS -1 CELLS 2 DEPTH 1" \
  "TYPE PLATE THICKNESS 0.01 CELLS 0 DEPTH 1" \
  "TYPE PLATE THICKNESS 0.01 CELLS 1.5 DEPTH 1" \
  "TYPE PLATE THICKNESS 0.01 CELLS 10001 DEPTH 1" \
  "TYPE PLATE THICKNESS 0.01 CELLS 2 DEPTH 0" \
  "TYPE PLATE THICKNESS 0.01 CELLS 2 DEPTH 1 INNER_RADIUS 0.005" \
  "TYPE PLATE THICKNESS 0.01 CELLS 2 DEPTH 1 PERIMETER_PRIMARY -1" \
  "TYPE TUBE THICKNESS 0.01 CELLS 2 INNER_RADIUS -1" \
  "TYPE PLATE THICKNESS 0.01+t CELLS 2 DEPTH 1"; do
  if ${feenox} ${dir}/thermal-hydraulics-exchanger-setup.fee "${geometry}"; then
    echo "expected an error for invalid exchanger geometry: ${geometry}"
    exit 1
  fi
done
for coefficients in "H_PRIMARY 0 H_SECONDARY 1e4" "H_PRIMARY 1e4 H_SECONDARY -1" "H_PRIMARY 1e4+x H_SECONDARY 1e4" "H_PRIMARY 1e4"; do
  if ${feenox} ${dir}/thermal-hydraulics-exchanger-setup.fee "TYPE PLATE THICKNESS 0.01 CELLS 2 DEPTH 1" relap7-heat-exchanger.msh "${coefficients}"; then
    echo "expected an error for invalid exchanger coefficients: ${coefficients}"
    exit 1
  fi
done
if diagnostic=$(${feenox} ${dir}/thermal-hydraulics-exchanger-setup.fee "TYPE PLATE THICKNESS 0.01 CELLS 2 DEPTH 1" relap7-heat-exchanger-mismatch.msh 2>&1); then
  echo "expected an error for nonmatching axial stations"
  exit 1
fi
echo "${diagnostic}" | grep -q "matching.*axial" || exit 1

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
answerzero thermal-hydraulics-pump-series.fee 1e-6
exitifwrong $?
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
