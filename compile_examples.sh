#!/bin/bash
# Compile each library example as a test

set -xe

PLATFORMS="photon electron"
EXAMPLES=`ls examples`

for platform in $PLATFORMS; do
  for example in $EXAMPLES; do
    particle --no-update-check compile $platform examples/$example
  done
done
