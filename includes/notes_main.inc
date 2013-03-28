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

function notes_complete_command_export()
{
    if [ "$#" == 0 ]
    then
        local notes_files=$(find -type f -iname [0-9].txt | sort -n )
        for file in ${notes_files[@]}
        do
            local id=$(basename $file ".txt")
            echo "${id}"
        done
    else
        # list files in work dir
        pushd ${WORK_DIR} > /dev/null
        find  -maxdepth 1  -iname "*.html"
    fi
    popd > /dev/null
    exit 0;
}
function notes_complete_command_delete()
{
    local notes_files=$(find -type f -iname [0-9].txt | sort -n )
    for file in ${notes_files[@]}
    do
        local id=$(basename $file ".txt")
        echo "${id}"
    done
    popd > /dev/null
    exit 0;
}

function notes_complete_command_edit()
{
    local notes_files=$(find -type f -iname [0-9].txt | sort -n )
    for file in ${notes_files[@]}
    do
        local id=$(basename $file ".txt")
        echo "${id}"
    done
    popd > /dev/null
    exit 0;
}

function notes_complete_command_view()
{
    local notes_files=$(find -type f -iname [0-9].txt | sort -n )
    for file in ${notes_files[@]}
    do
        local id=$(basename $file ".txt")
        echo "${id}"
    done
    popd > /dev/null
    exit 0;
}

function notes_complete_command_list()
{
    local NOTES_FILES=$(find -type f -iname [0-9].txt | sort -n )
    for file in ${NOTES_FILES[@]}
    do
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
    popd > /dev/null
    exit 0;
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

function notes_main_run_export()
{
    local notes_file;
    notes_get_from_id "$1" notes_file
    if [ -z "$1" ]
    then
        note_error "Usage: export <id> <filename>"
        exit 1;
    fi

    local filename="$2"
    if [ -z "$filename" ]
    then
        note_error "Usage: export <id> <filename>"
        exit 1;
    fi

    pushd "${WORK_DIR}"
    __notes_view_generate_html "${NOTE_DIR}/${notes_file}" "${filename}"
    popd
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
    # ASCIIDOC
    if [ -z "$ASCIIDOC" ] || [ ! -x $(which $ASCIIDOC) ]
    then
        notes_error "Failed to find a valid asciidoc: $ASCIIDOC"
        exit 1;
    fi
    # EDITOR
    if [ -z "$EDITOR" ] || [ ! -x $(which $EDITOR) ]
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
