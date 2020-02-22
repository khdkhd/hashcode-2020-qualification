#!/bin/bash

# stop script with non-zero exit code if anything go wrong
set -e

# stop script with non-zero exit code when trying to reference an undefined
# variable
set -u

# If any command in a pipeline fails, that return code will be used as the
# return code of the whole pipeline
set -o pipefail

SOURCE_DIR="${SOURCE_DIR:-$PWD}"
BUILD_DIR="${BUILD_DIR:-$SOURCE_DIR/BuildRelease}"

if ! [ -d "$BUILD_DIR" ];
then
    mkdir "$BUILD_DIR"
fi

pushd "$BUILD_DIR"
cmake --Wno-dev -DCMAKE_BUILD_TYPE=Release "$SOURCE_DIR"
cmake --build .
popd

export BOOKS_OUTPUT_DIR="$SOURCE_DIR/solutions/$BOOKS_ALGO/$(date -u +'%Y-%m-%dT%H:%M:%SZ')"
# export BOOKS_OUTPUT_DIR="$SOURCE_DIR/solutions/$BOOKS_ALGO"

mkdir -p "$BOOKS_OUTPUT_DIR"
INPUT_FILES=$(IFS=$'\n'; find "$SOURCE_DIR/inputs" -type f)

2>&1 "$BUILD_DIR/hashcode-2020-books" $BOOKS_ALGO ${INPUT_FILES[@]} | grep "score:"
