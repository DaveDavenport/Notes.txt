#!/bin/bash

WORK_DIR="${PWD}"

##
# Load config file.
##
. ~/.notes.config

##
# Test if config loaded fine 
##
if [ $? != 0 ]
then
    echo "Failed to find config file: ~/.notes/config."
    exit 1;
fi


###
# Do not edit below this line.
###

# load include files
source ${INCLUDE_DIR}/notes_globals.inc
source ${INCLUDE_DIR}/notes_functions.inc
source ${INCLUDE_DIR}/notes_main.inc

# validate the config
notes_validate_config

# go to the Notes directory.
pushd "${NOTE_DIR}" > /dev/null

# Handle autocomplete
if [ "$1" == "--complete" ]
then
    shift;
    # Complete base command
    notes_complete_commands "$@"
    popd > /dev/null;
    exit 0;
fi

# check vcs directory
notes_vcs_validate_dir "${NOTE_DIR}"

# Check temp directory.
notes_check_directory "$TEMP_DIR"

# When online, check updates
if [ "${HAS_INET}" == 1 ]
then
    notes_info "Check for updates"
    notes_vcs_check_updates;
fi

notes_main_run_commands "${@}"

# commit results, this function checks if anything changed.
notes_vcs_commit_changes

# When on-line, push updates
if [ "${HAS_INET}" == 1 ]
then
    notes_info "Pushing changes"
    notes_vcs_push_changes;
fi

# return.
popd > /dev/null # ${NOTE_DIR} 
