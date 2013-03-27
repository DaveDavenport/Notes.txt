#!/bin/bash

NOTE_DIR=~/Notes/

source ./notes_functions.inc

pushd "${NOTE_DIR}"

# Get a list of NOTES files.
NOTES_FILES=$(find -type f -iname *.txt)


for a in ${NOTES_FILES}
do
get_note_name "$a" 
done

popd # ${NOTE_DIR}
