#!/bin/bash

. config.inc


###
# Do not edit below this line.
###
source ./includes/notes_functions.inc

notes_validate_config
# go to the Notes directory.
pushd "${NOTE_DIR}" > /dev/null

# Check temp directory.
notes_check_directory "$TEMP_DIR"

# Check updates
notes_check_updates;


# Get a list of NOTES files.
NOTES_FILES=$(find -type f -iname *.txt)


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
        #List/other
        *)
        ;;
    esac
    # Next argument
    shift
done

# return.
popd > /dev/null # ${NOTE_DIR} 
