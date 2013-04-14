#!/bin/bash

# Copyright © 2013, Qball Cow and contributors
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
# 
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#       
#     * Neither the name of Qball Cow nor the
#       names of contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY Qball Cow ''AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL Qball Cow BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Store the current directory. Used for exporting.
WORK_DIR="${PWD}"

# check file exists function.
function notes_check_file()
{
    if [ ! -f ${1} ]
    then
        echo "Could not find file: ${1}" 2>&1
        exit 1;
    fi
}

##
# Load config file.
##
CONFIG_FILE=${HOME}/.notes.config
notes_check_file "${CONFIG_FILE}"
. ${CONFIG_FILE}

# Test if config loaded fine 
if [ $? != 0 ]
then
    echo "Failed to find config file: ~/.notes/config."
    exit 1;
fi

# load include files
notes_check_file "${INCLUDE_DIR}/notes_colors.inc"
notes_check_file "${INCLUDE_DIR}/notes_debug.inc"
notes_check_file "${INCLUDE_DIR}/notes_globals.inc"
notes_check_file "${INCLUDE_DIR}/notes_functions.inc"
notes_check_file "${INCLUDE_DIR}/notes_main.inc"
notes_check_file "${INCLUDE_DIR}/notes_vcs.inc"

source ${INCLUDE_DIR}/notes_colors.inc
source ${INCLUDE_DIR}/notes_debug.inc
source ${INCLUDE_DIR}/notes_globals.inc
source ${INCLUDE_DIR}/notes_functions.inc
source ${INCLUDE_DIR}/notes_main.inc
source ${INCLUDE_DIR}/notes_vcs.inc

# validate the config
notes_validate_config

# go to the Notes directory.
pushd "${NOTE_DIR}" > /dev/null

# Load global list.
notes_global_load_list;

# Handle autocomplete
if [ "$1" == "--complete" ]
then
    shift;
    # Complete base command
    notes_complete_commands "$@"
    popd > /dev/null;
    exit 0;
fi

notes_timestamp "Start"

notes_timestamp "load list"

# check vcs directory
notes_vcs_validate_dir "${NOTE_DIR}"

notes_timestamp "VCS validate"
# Check temp directory.
notes_check_directory "$TEMP_DIR"

notes_timestamp "Check Directory"
# When online, check updates
if [ "${HAS_INET}" == 1 ]
then
    notes_info "Check for updates"
    notes_vcs_check_updates;
    notes_timestamp "Check Updates"
fi

notes_main_run_commands "${@}"
notes_timestamp "Run command"

# commit results, this function checks if anything changed.
notes_vcs_commit_changes

# When on-line, push updates
if [ "${HAS_INET}" == 1 ]
then
    notes_info "Pushing changes"
    notes_vcs_push_changes;
    notes_timestamp "Push Changes"
fi

# return.
popd > /dev/null # ${NOTE_DIR} 
notes_timestamp "End"
