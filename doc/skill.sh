#!/bin/bash -e
# Build an agent skill (a SKILL.md plus supporting files) for LLM agents that
# write FeenoX inputs, out of the same sources as the manual:
#
#   skill/SKILL.md                 from skill.md, with the FeenoX version
#   skill/syntax.md                from input-syntax.md
#   skill/command-line.md          from help.md
#   skill/reference/INDEX.md       every entry with its one-line summary and file
#   skill/reference/<kind>/<NAME>.md                 keywords, functions, ...
#   skill/reference/problems/<problem>/<kind>/<NAME>.md
#   skill/EXAMPLES.md              the examples and tests grouped by PROBLEM
#   skill/examples/, skill/tests/  their *.fee and *.geo files (tracked ones)
#
# It reads the reference-*.md files that reference.sh writes, so make.sh calls
# it after them. One small file per entry lets an agent read only what it needs.
# Each entry lists (some of) the example and test files that use it, instead
# of the links to Github of the HTML manual. Figures are left out, and so are
# the tests that are expected to fail (xfail-*, *wrong*): they show what not
# to write.

for i in awk sed git; do
 if [ -z "$(which $i)" ]; then
  echo "error: ${i} not installed"
  exit 1
 fi
done

if [ ! -e "skill.md" ] || [ ! -e "reference-kw.md" ]; then
  echo "error: execute from doc directory after reference.sh"
  exit 1
fi

out=skill
rm -rf ${out}
mkdir -p ${out}/reference

version=$(git describe --tags --dirty 2>/dev/null || echo "(unknown version)")
sed "s/\`VERSION\`/${version}/" skill.md > ${out}/SKILL.md
sed 's_(feenox-manual.md#reference)_(reference/INDEX.md)_' input-syntax.md > ${out}/syntax.md
{ echo "# Command-line options"; echo; cat help.md; } > ${out}/command-line.md

# the inputs of the examples and tests (only tracked files, like the manual)
inputs=$(mktemp)
git ls-files ../examples ../tests | grep -E '\.(fee|geo)$' \
  | grep -v -E '(^|/)xfail-|wrong' | sed 's_^\.\./__' | sort > ${inputs}
(cd .. && xargs -a ${inputs} cp --parents -t doc/${out})

# one file per "#  `NAME`" section of reference-*.md, and a tab-separated list
# area, kind, name, summary, file (relative to reference/) for the index
split() {
  local src=$1 area=$2 kind=$3
  local dir=${kind}
  if [ -n "${area}" ]; then
    dir=problems/${area}/${kind}
  fi
  awk -v dir="${dir}" -v base="${out}/reference" -v area="${area}" -v kind="${kind}" -v max=8 '
    FNR == NR { copied[$0] = 1; next }
    function flush(  file, title) {
      if (name == "") return
      file = dir "/" safe(name) ".md"
      title = (area != "") ? name " (" area " problems)" : name
      gsub(/\n\n\n+/, "\n\n", body)
      sub(/^\n+/, "", body); sub(/\n+$/, "", body)
      if (nex > 0) {
        body = body "\n\n## Examples of usage\n\n" exs
        if (nex > max) body = body "- ... and " nex - max " more in EXAMPLES.md, which lists the keywords each file uses\n"
        sub(/\n+$/, "", body)
      }
      printf "# `%s`\n\n%s\n", title, body > (base "/" file)
      close(base "/" file)
      s = summary; gsub(/[ \t]+/, " ", s); sub(/^ /, "", s); sub(/ $/, "", s)
      if (match(s, /\.( |$)/)) s = substr(s, 1, RSTART)
      printf "%s\t%s\t%s\t%s\t%s\n", area, kind, name, substr(s, 1, 200), file
      name = ""
    }
    function safe(n) { gsub(/[^A-Za-z0-9_.+-]/, "_", n); return n == "" ? "_" : n }
    /^#  `.*`/ {
      flush()
      name = $0; sub(/^#  `/, "", name); sub(/`.*$/, "", name)
      body = ""; summary = ""; indesc = 0; skip = 0; exs = ""; nex = 0
      next
    }
    name == "" { next }
    # web-only blocks: the list of examples on Github, turned into local files
    /^:{3,} *\{.*only-in-format.*html/ { skip = 1; next }
    skip && /^:{3,} *$/ { skip = 0; next }
    skip && match($0, /blob\/main\/[^)#]+#L[0-9]+/) {
      ref = substr($0, RSTART + 10, RLENGTH - 10)
      path = ref; sub(/#L.*/, "", path)
      line = ref; sub(/.*#L/, "", line)
      if (path in copied && ++nex <= max) exs = exs "- `" path "`, line " line "\n"
      next
    }
    skip { next }
    # the italic description is the one-line summary
    /^:{3,} *\{.*fst-italic/ { indesc = 1; next }
    /^:{3,}/ { indesc = 0; next }
    /^!\[.*\]\(figures\// { next }
    { sub(/[ \t]+$/, ""); if (indesc) summary = summary " " $0; body = body $0 "\n" }
    END { flush() }
  ' ${inputs} "${src}"
}

kinds="kw:keywords fn:functions fu:functionals fv:vector-functions va:variables pb:problem bc:boundary-conditions pr:properties re:results"

list=$(mktemp)
for f in reference-*.md; do
  stem=${f#reference-}
  stem=${stem%.md}
  tag=${stem##*-}
  area=""
  if [ "${stem}" != "${tag}" ]; then
    area=${stem%-*}
  fi
  # keywords and variables of all PDEs and of DAEs are general ones
  if [ "${area}" = "pde" ] || [ "${area}" = "dae" ]; then
    area=""
  fi
  kind=""
  for k in ${kinds}; do
    if [ "${k%%:*}" = "${tag}" ]; then
      kind=${k#*:}
    fi
  done
  if [ -z "${kind}" ]; then
    continue
  fi
  if [ -n "${area}" ]; then
    mkdir -p ${out}/reference/problems/${area}/${kind}
  else
    mkdir -p ${out}/reference/${kind}
  fi
  split ${f} "${area}" ${kind} >> ${list}
done

# the index: general entries first, then each problem; kinds in a fixed order
awk -F'\t' -v kinds="${kinds}" -v version="${version}" '
  BEGIN {
    n = split(kinds, k, " ")
    for (i = 1; i <= n; i++) { split(k[i], p, ":"); order[p[2]] = i }
    title["keywords"] = "Keywords"; title["functions"] = "Functions"
    title["functionals"] = "Functionals"; title["vector-functions"] = "Functions of vectors"
    title["variables"] = "Special variables"; title["problem"] = "Problem"
    title["boundary-conditions"] = "Boundary conditions"
    title["properties"] = "Material properties"; title["results"] = "Results"
  }
  {
    key = sprintf("%s%s\t%02d", ($1 == "" ? "0" : "1"), $1, order[$2])
    if (!(key in seen)) { seen[key] = 1; keys[++m] = key; head[key] = title[$2] ($1 == "" ? "" : " of `" $1 "` problems") }
    lines[key] = lines[key] sprintf("- `%s`: %s (%s)\n", $3, $4, $5)
  }
  END {
    print "# FeenoX reference: index"
    print ""
    print "Generated by skill.sh from the documentation of FeenoX " version "; do not edit."
    print "Each entry: its summary and the file with its full description and syntax."
    print ""
    # insertion sort of the section keys
    for (i = 2; i <= m; i++) { x = keys[i]; for (j = i - 1; j >= 1 && keys[j] > x; j--) keys[j+1] = keys[j]; keys[j+1] = x }
    for (i = 1; i <= m; i++) { print "## " head[keys[i]]; print ""; printf "%s", lines[keys[i]]; print "" }
  }
' ${list} > ${out}/reference/INDEX.md

# the examples and tests by problem: what each one uses, and its mesh's .geo
problems=$(ls reference-*-pb.md | sed 's/reference-//;s/-pb.md//' | tr '\n' ' ')
grep '\.fee$' ${inputs} | (cd ${out} && xargs awk -v problems="${problems}" -v version="${version}" '
  function done_file(  key, d) {
    if (file == "") return
    key = (problem == "") ? "~" : problem
    d = "- `" file "`"
    if (dim != "") d = d ", " dim
    if (geo != "") d = d ", mesh from `" geo "`"
    if (comment != "") d = d ": " comment
    if (kws != "") d = d " (" kws ")"
    entries[key] = entries[key] d "\n"; count[key]++
  }
  BEGIN { n = split(problems, p, " "); for (i = 1; i <= n; i++) known[p[i]] = 1 }
  FNR == 1 {
    done_file()
    file = FILENAME; problem = ""; dim = ""; geo = ""; comment = ""; kws = ""; delete seen
    front = ($0 ~ /^---/)
  }
  front { if (FNR > 1 && /^(---|\.\.\.)[ \t]*$/) front = 0; next }
  /^[ \t]*#/ {
    if (comment == "" && !/^#!/) { comment = $0; sub(/^[ \t]*#+[ \t]*/, "", comment); sub(/[ \t]+$/, "", comment) }
    next
  }
  match($0, /^[ \t]*[A-Z][A-Z_]+([ \t]|$)/) {
    kw = $1
    if (!(kw in seen)) { seen[kw] = 1; kws = kws (kws == "" ? "" : " ") kw }
    if (toupper(kw) == "PROBLEM" && problem == "" && (tolower($2) in known)) {
      problem = tolower($2)
      for (i = 3; i <= NF; i++) if ($i ~ /^[123][dD]$/) dim = toupper($i)
    }
    for (i = 2; i < NF; i++) if (($i == "MESH" || kw == "READ_MESH") && geo == "") {
      m = (kw == "READ_MESH") ? $2 : $(i + 1)
      g = m; sub(/\.msh$/, ".geo", g)
      dir = file; sub(/[^\/]*$/, "", dir)
      if (g != m && (getline tmp < (dir g)) > 0) geo = dir g
      close(dir g)
      if (kw == "READ_MESH") break
    }
  }
  END {
    done_file()
    print "# FeenoX examples and tests, by problem"
    print ""
    print "Generated by skill.sh from FeenoX " version "; do not edit. Every input here"
    print "runs (tests that are expected to fail are left out). Start a new input from"
    print "one with the same `PROBLEM`: read it, and the `.geo` its mesh comes from for"
    print "the names of the physical groups. Each line: the file, its dimension, the"
    print "Gmsh input of its mesh, its first comment, and the keywords it uses."
    for (i = 1; i <= n; i++) if (p[i] in count) {
      print ""; print "## `PROBLEM " p[i] "` (" count[p[i]] ")"; print ""; printf "%s", entries[p[i]]
    }
    if ("~" in count) {
      print ""; print "## No `PROBLEM`: algebra, functions, ODEs and DAEs, included files (" count["~"] ")"
      print ""; printf "%s", entries["~"]
    }
  }
') > ${out}/EXAMPLES.md

echo "  $(wc -l < ${list}) entries, $(grep -c '\.fee$' ${inputs}) example inputs in ${out}/"
rm -f ${list} ${inputs}
