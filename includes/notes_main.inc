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

##############################
## Completion Functions     ##
##############################


function notes_complete_command_convert()
{
    if [ "$#" == 0 ]
    then
        for format in ${CONVERT_FORMATS[@]}
        do
            echo "${format}"
        done
    elif [ "$#" == 1 ]
    then
        echo "${!GLOBAL_NOTES_LIST[@]}"
    fi
    popd > /dev/null
    exit 0;
}
function notes_complete_command_export()
{
    if [ "$#" == 0 ]
    then
        echo "raw"
        for format in ${MARKUP_OUTPUT_FORMATS[@]}
        do
            echo "${format}"
        done
    else if [ "$#" == 1 ]
    then
        echo "${!GLOBAL_NOTES_LIST[@]}"
    elif [ "$#" == 2 ]
    then
        if [ "${1}" == "raw" ]
        then
            # list files in work dir
            pushd ${WORK_DIR} > /dev/null
            find  -maxdepth 1  -iname "*.txt"
            popd > /dev/null
        else
            # list files in work dir
            pushd ${WORK_DIR} > /dev/null
            find  -maxdepth 1  -iname "*"
            popd > /dev/null
        fi
    fi
    fi
    popd > /dev/null
    exit 0;
}
function notes_complete_command_move()
{
    if [ "$#" == 0 ]
    then
        echo "${!GLOBAL_NOTES_LIST[@]}"
    else if [ "$#" == 1 ]
    then
        for index in ${!GLOBAL_NOTES_LIST[@]} 
        do
            local file=${GLOBAL_NOTES_LIST[index]}
            local category
            __notes_category_name "$file" category
            echo "${category}"
        done
    fi
    fi
    popd > /dev/null
    exit 0;
}

function notes_complete_command_delete()
{
    echo "${!GLOBAL_NOTES_LIST[@]}"
    popd > /dev/null
    exit 0;
}

function notes_complete_command_archive()
{
    echo "${!GLOBAL_NOTES_LIST[@]}"
    popd > /dev/null
    exit 0;
}

function notes_complete_command_edit()
{
    echo "${!GLOBAL_NOTES_LIST[@]}"
    popd > /dev/null
    exit 0;
}

function notes_complete_command_view()
{
    echo "${!GLOBAL_NOTES_LIST[@]}"
    popd > /dev/null
    exit 0;
}

function notes_complete_command_add()
{
    if [ $# == 0 ]
    then
        for index in ${!GLOBAL_NOTES_LIST[@]} 
        do
            local file=${GLOBAL_NOTES_LIST[index]}
            local category
            __notes_category_name "$file" category
            echo "${category}"
        done
    fi
    popd > /dev/null
    exit 0;
}

function notes_complete_command_list()
{
    for index in ${!GLOBAL_NOTES_LIST[@]} 
    do
        local file=${GLOBAL_NOTES_LIST[index]}
        local category
        __notes_category_name "$file" category
        echo "${category}"
    done
    popd > /dev/null
    exit 0;
}

function notes_complete_commands()
{
    # List commands.
    if [ ${#} == 0 ]
    then
        for command in "${COMMANDS[@]}"
        do
            echo "$command"
        done
        return;
    fi

    # Automatically find, if available, the completion function.
    local command=notes_complete_command_"${1}" 
    if [ "$(type -t $command)" == "function" ]
    then
        shift
        $command "${@}"
    fi
}



##################################
# Parse commandline
##################################

function notes_main_run_view()
{
    # Get next argument 
    note="" 
    notes_get_from_id "$1" note
    notes_view "$note" 
}

function notes_main_run_push()
{
    notes_info "Pushing changes"
    notes_vcs_push_changes;
}

function notes_main_run_pull()
{
    # Check updates
    notes_info "Check for updates"
    notes_vcs_check_updates;
}

function notes_main_run_add()
{
    if [ -z "$1" ]
    then
        notes_error "You need to specify a category: add <category>"
        exit 1;
    fi
    # Add a note
    notes_add $1;
}

function notes_main_run_list()
{
    notes_info "Listing notes:"
    notes_list "$@";
}

function notes_main_run_edit()
{
    # Get next argument 
    note="" 
    notes_get_from_id "$1" note
    notes_edit "$note"
}

function notes_main_run_delete()
{
    notes_info "Deleting note"
    note="" 
    notes_get_from_id "$1" note
    notes_delete "$note"
}

function notes_main_run_convert()
{
    local note_file;

    ##
    # Get input
    ##
    notes_get_from_id "$2" note_file
    if [ -z "${note_file}" ]
    then
        notes_error "Usage: convert <format> <id>"
        exit 1;
    fi

    notes_info "Exporting ${note_file}"
    local convert_format="$1"

    echo ${CONVERT_COMMAND}
    eval ${CONVERT_COMMAND} 
    if [ $? != 0 ]
    then
        notes_error "Failed to create $1 file"
        exit 1;
    fi
}
function notes_main_run_export()
{
    local notes_file;

    ##
    # Get input
    ##
    notes_get_from_id "$2" notes_file
    if [ -z "$2" ]
    then
        note_error "Usage: export <id> <filename>"
        exit 1;
    fi

    ##
    # Get output
    ##
    local filename="$3"
    if [ -z "$filename" ]
    then
        note_error "Usage: export <id> <filename>"
        exit 1;
    fi

    ##
    # Check style of conversion.
    ##
    if [ "$1" == "raw" ]
    then
        pushd "${WORK_DIR}" > /dev/null
        notes_info "Exporting to ${filename}"
        cp "${NOTE_DIR}/${notes_file}" "${filename}"
        popd > /dev/null
    else
        pushd "${WORK_DIR}" > /dev/null
        notes_info "Exporting to ${filename}"
        local output_format="$1"
        eval $MARKUP_PARSER "${NOTE_DIR}/${notes_file}" > "${filename}"
        if [ $? != 0 ]
        then
            notes_error "Failed to create $1 file"
            exit 1;
        fi
        popd > /dev/null
    fi
}

function notes_main_run_clean()
{
    notes_info "Cleaning temporary files"
    if [ -d "${TEMP_DIR}" ]
    then
        notes_print "Are you want to clean out temporary files?"
        echo -n "(y/n):"
        local answer="n"
        read answer
        if [ $answer == "y" ]
        then
            rm -r "${TEMP_DIR}"
        fi
    fi
}

function notes_main_run_move()
{
    local note_id="$1"
    local new_category="$2"
    local note_filename;
    # Get note file
    notes_get_from_id "$note_id" note_filename;
    # validate category name.
    notes_validate_category "$new_category";

    local PROJECT=${new_category//./\/}
    notes_check_directory "${PROJECT}"

    notes_vcs_move_file "${note_filename}" "./${PROJECT}/${note_id}.txt"
}

function notes_main_run_commands()
{
    # Automatically find, if available, the completion function.
    local command=notes_main_run_"${1}" 
    
    if [ "$#" -gt 0 ]
    then
        if [ "$(type -t $command)" == "function" ]
        then
            shift
            $command "${@}"
        else
            notes_error "Function ${1} note found"
            exit 1;
        fi
    else
        notes_main_run_list
    fi
}

# Validate the config.
function notes_validate_config()
{
    # GIT
    if [ -z "$GIT" ] || [ ! -x $(which $GIT) ]
    then
        notes_error "Failed to find a valid GIT: $GIT"
        exit 1;
    fi
    # MARKUP_PARSER
    CMD=$(basename "$MARKUP_PARSER")
    FN=$(expr match "$CMD" '^\([a-zA-Z0-9]*\)')
    if [ -z $FN ] || [ ! -x $(which $FN) ]
    then
        notes_error "Failed to find a valid markup parser: $FN"
        exit 1;
    fi
    # EDITOR
    CMD=$(basename "$EDITOR")
    FN=$(expr match "$CMD" '^\([a-zA-Z0-9]*\)')
    if [ -z $FN ] || [ ! -x $(which $FN) ]
    then
        notes_error "Failed to find a valid editor: $EDITOR"
        exit 1;
    fi
    # Directory
    if [ ! -d "$NOTE_DIR" ] 
    then
        notes_error "Failed to find the NOTE directory: $NOTE_DIR"
        exit 1;
    fi

    notes_check_directory "${NOTE_DIR}/${ARCHIVE_DIR}"

    # md5sum
    if [ ! -x $(which md5sum) ]
    then
        notes_error "Failed to find md5sum"
        exit 1;
    fi

    # grep 
    if [ ! -x $(which grep) ]
    then
        notes_error "Failed to find grep"
        exit 1;
    fi


    # sed 
    if [ ! -x $(which sed) ]
    then
        notes_error "Failed to find sed"
        exit 1;
    fi
}


function notes_check_file()
{
    if [ ! -f "$1" ]
    then
        notes_error "Could not find file: ${1}"
    fi
}


function notes_main_run_archive()
{
    local note_id="$1"
    local note_filename;
    # Get note file
    notes_get_from_id "$note_id" note_filename;

    PROJECT=$(dirname "${note_filename}");
    echo ${PROJECT};
    # validate category name.
    notes_check_directory "${ARCHIVE_DIR}/${PROJECT}"

    notes_vcs_move_file "${note_filename}" "./${ARCHIVE_DIR}/${note_filename}"
}
