#!/bin/bash
# Publish a new Particle library version when a tagged build passes

# Nothing to do if no tag
[ -z $TRAVIS_TAG ] && exit

# Check if the tag matches the library version
[ -z $(grep -e "^version=$TRAVIS_TAG$" library.properties) ] && echo "libraries.properties is not at version $TRAVIS_TAG" && exit 1

# Publish
particle --no-update-check library publish
