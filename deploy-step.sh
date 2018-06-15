#!/bin/sh

# simple script which can be used as deployment step in qt creator
# $1 is expected to be target directory


echo Deploying `pwd` to $1

# copy folders ...
cp -rf \
 help \
 images \
 java \
 qss \
 translations \
 $1

# ... and files required for execution
cp -f \
 changelog.txt \
 license.html \
 shortcuts.txt \
 $1

echo Finished deployment..