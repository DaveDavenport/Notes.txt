# List of colors.
# TODO: Move into notes_colors.inc
NOTE_COLOR_RESET="\e[0m"
NOTE_COLOR_BLACK="\e[0;30m"
NOTE_COLOR_RED="\e[0;31m"
NOTE_COLOR_GREEN="\e[0;32m"
NOTE_COLOR_YELLOW="\e[0;33m"
NOTE_COLOR_BLUE="\e[0;34m"
NOTE_COLOR_MAGENTA="\e[0;35m"
NOTE_COLOR_CYAN="\e[0;36m"
NOTE_COLOR_WHITE="\e[0;37m"
NOTE_COLOR_BLACK_BOLD="\e[1;30m"
NOTE_COLOR_RED_BOLD="\e[1;31m"
NOTE_COLOR_GREEN_BOLD="\e[1;32m"
NOTE_COLOR_YELLOW_BOLD="\e[1;33m"
NOTE_COLOR_BLUE_BOLD="\e[1;34m"
NOTE_COLOR_MAGENTA_BOLD="\e[1;35m"
NOTE_COLOR_CYAN_BOLD="\e[1;36m"

##
# Print an information message
##
function notes_info()
{
    echo -e $NOTE_COLOR_BLUE"$1"$NOTE_COLOR_RESET >&1
}

##
# Print an error message
##
function notes_error()
{
    echo -e $NOTE_COLOR_RED_BOLD"$1"$NOTE_COLOR_RESET >&2
}


function notes_get_name()
{
    HEADING=$(head -n 1 "$1")
    echo ${HEADING}
}

function notes_view()
{
    # Compile note

    # View it.
    $EDITOR
}


##
# Check directory and create
##
function notes_check_directory()
{
    if [ ! -d "$1" ]
    then
        mkdir -p "$1"
        if [ $? != 0 ]
        then
            notes_error "Failed to create directory";
            exit 1;
        fi
    fi
}


function notes_check_updates()
{
    notes_info "Check for updates"
    $VCS_UPDATE
}
