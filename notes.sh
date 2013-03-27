#!/bin/bash

WORK_DIR="${PWD}"

. ~/.notes.config

if [ $? != 0 ]
then
    echo "Failed to find config file: ~/.notes/config."
    exit 1;
fi


COMMANDS=( "edit" "view" "push" "pull" "add" "list" "delete" "export")

###
# Do not edit below this line.
###
source ${INCLUDE_DIR}/notes_functions.inc
source ${INCLUDE_DIR}/notes_main.inc

notes_validate_config


# go to the Notes directory.
pushd "${NOTE_DIR}" > /dev/null

# Handle autocomplete
#TODO: make function.
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


if [ "${HAS_INET}" == 1 ]
then
    notes_info "Check for updates"
    notes_vcs_check_updates;
fi

notes_main_run_commands "${@}"

notes_vcs_commit_changes
if [ "${HAS_INET}" == 1 ]
then
    notes_info "Pushing changes"
    notes_vcs_push_changes;
fi

# return.
popd > /dev/null # ${NOTE_DIR} 
