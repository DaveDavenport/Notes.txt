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
