#!/bin/bash

. ~/.notes.config

if [ $? != 0 ]
then
    echo "Failed to find config file: ~/.notes/config."
    exit 1;
fi


COMMANDS=( "edit" "view" "push" "pull" "add" "list" )

###
# Do not edit below this line.
###
source ${INCLUDE_DIR}/notes_functions.inc

notes_validate_config


# go to the Notes directory.
pushd "${NOTE_DIR}" > /dev/null

# Handle autocomplete
#TODO: make function.
if [ "$1" == "--complete" ]
then
    if [ $# == 2 ]
    then
        for command in "${COMMANDS[@]}"
        do
            echo "$command"
        done
    fi
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

while (( "$#"))
do
    arg="$1"
    case "$arg" in 
        # Edit
        edit)
            # Get next argument 
            shift
            note="" 
            notes_get_from_id "$1" note
            notes_edit "$note"
        ;;
        # View
        view)
            # Get next argument 
            shift
            note="" 
            notes_get_from_id "$1" note
            notes_view "$note" 
        ;;
        push)
            notes_info "Pushing changes"
            notes_vcs_push_changes;
        ;;
        pull)
            # Check updates
            notes_info "Check for updates"
            notes_vcs_check_updates;
        ;;
        add)
            shift
            if [ -z "$1" ]
            then
                notes_error "You need to specify a category: add <category>"
                exit 1;
            fi
            # Add a note
            notes_add $1;
        ;;
        #List/other
        *)
            notes_info "Listing notes:"
            notes_list;
            popd > /dev/null
            exit 0;
        ;;
    esac
    # Next argument
    shift
done

notes_vcs_commit_changes
if [ "${HAS_INET}" == 1 ]
then
    notes_info "Pushing changes"
    notes_vcs_push_changes;
fi

# return.
popd > /dev/null # ${NOTE_DIR} 
