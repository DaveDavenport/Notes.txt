#!/bin/bash

NOTE_DIR=~/Notes/
TEMP_DIR=~/Notes/.temp/
VCS_UPDATE="git pull"


###
# Do not edit below this line.
###
source ./notes_functions.inc

# go to the Notes directory.
pushd "${NOTE_DIR}" > /dev/null

# Check temp directory.
notes_check_directory "$TEMP_DIR"

# Check updates
notes_check_updates;


# Get a list of NOTES files.
NOTES_FILES=$(find -type f -iname *.txt)

notes_info "List of notes"
for a in ${NOTES_FILES}
do
notes_get_name "$a" 
done

# return.
popd > /dev/null # ${NOTE_DIR} 
