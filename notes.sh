#!/bin/bash

. config.inc


###
# Do not edit below this line.
###
source ${INCLUDE_DIR}/notes_functions.inc

notes_validate_config


# go to the Notes directory.
pushd "${NOTE_DIR}" > /dev/null

# check vcs directory
notes_vcs_validate_dir "${NOTE_DIR}"

# Check temp directory.
notes_check_directory "$TEMP_DIR"

# Check updates
notes_check_updates;


while (( "$#"))
do
    arg="$1"
    case "$arg" in 
        # Edit
        e*)
            # Get next argument 
            shift
            note="" 
            notes_get_from_id "$1" note
            notes_edit "$note"
        ;;
        # View
        v*)
            # Get next argument 
            shift
            note="" 
            notes_get_from_id "$1" note
            notes_view "$note" 
        ;;
        p*)
            notes_vcs_push_changes;
        ;;
        #List/other
        *)
            notes_info "Listing nodes:"
            notes_list;
            popd > /dev/null
            exit 0;
        ;;
    esac
    # Next argument
    shift
done

notes_vcs_commit_changes

# return.
popd > /dev/null # ${NOTE_DIR} 
