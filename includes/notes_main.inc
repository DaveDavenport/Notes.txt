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
        popd > /dev/null
    fi
}
function notes_complete_command_delete()
{
    local notes_files=$(find -type f -iname [0-9].txt | sort -n )
    for file in ${notes_files[@]}
    do
        local id=$(basename $file ".txt")
        echo "${id}"
    done
}

function notes_complete_command_edit()
{
    local notes_files=$(find -type f -iname [0-9].txt | sort -n )
    for file in ${notes_files[@]}
    do
        local id=$(basename $file ".txt")
        echo "${id}"
    done
}

function notes_complete_command_view()
{
    local notes_files=$(find -type f -iname [0-9].txt | sort -n )
    for file in ${notes_files[@]}
    do
        local id=$(basename $file ".txt")
        echo "${id}"
    done
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

