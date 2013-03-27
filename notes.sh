#!/bin/bash

NOTE_DIR=~/Notes/
TEMP_DIR=~/Notes/.temp/
ASCIIDOC=asciidoc
VCS_UPDATE="git pull"
VCS_COMMIT="git commit"
EDITOR=vim


###
# Do not edit below this line.
###
source ./notes_functions.inc

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
