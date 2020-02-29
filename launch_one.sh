#!/bin/bash

set -ex

BUILD_DIR="$PWD/Build"
SOLUTION_DIR="$PWD/solutions"

solver=$1;
file=$2;

# Build
mkdir -p "${BUILD_DIR}"
pushd "${BUILD_DIR}"
cmake ..
make
popd

# Run
now="$(date -u +'%Y-%m-%dT%H.%M.%SZ')"
problem_name=$(basename "${file}" | sed 's/\..*$//');
output="${SOLUTION_DIR}/${solver}/${now}_${problem_name}";
mkdir -p "${output}";

exec cat "${file}" | "${BUILD_DIR}/hashcode-2020-books" "${solver}" \
  > "${output}/${problem_name}.out" \
  2> >(tee -a "${output}/traces_${problem_name}.txt" >&2)

cp "cpp/solvers/${solver}.cpp" "${output}"
ln -s "${output}" "${SOLUTION_DIR}/${solver}/latest";
