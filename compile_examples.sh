#!/bin/bash
# Compile each library example as a test

set -xe
cd `dirname $0`
PLATFORMS="photon electron"
EXAMPLES=`ls examples`

for platform in $PLATFORMS; do
  for example in $EXAMPLES; do
    particle --no-update-check compile $platform examples/$example
  done
done
