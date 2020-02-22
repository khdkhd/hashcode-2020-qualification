#!/bin/bash

set -e

if [[ ! -d Build ]]
then
  mkdir Build
  cd Build
  cmake ..
fi

cd Build
make

cd ..

args=("$@")
arguments=$(IFS=, ; echo "${args[*]}")
current_date="$(date -u +'%Y-%m-%dT%H:%M:%SZ')"
mkdir -p "traces/${current_date}-${arguments}" "solutions/${current_date}-${arguments}"

for file in $(ls -1 ./inputs);
do
  output="$(date -u +'%Y-%m-%dT%H:%M:%SZ')_solution_$file";
  exec cat "./inputs/$file" | ./Build/hashcode-2020-books $@ > "solutions/${current_date}-${arguments}/${output}";
done;
