
# Include the VCS stuff
. ${INCLUDE_DIR}/notes_vcs.inc

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
NOTE_COLOR_WHITE_BOLD="\e[1;37m"
NOTE_COLOR_BLACK_BOLD="\e[1;30m"
NOTE_COLOR_RED_BOLD="\e[1;31m"
NOTE_COLOR_GREEN_BOLD="\e[1;32m"
NOTE_COLOR_YELLOW_BOLD="\e[1;33m"
NOTE_COLOR_BLUE_BOLD="\e[1;34m"
NOTE_COLOR_MAGENTA_BOLD="\e[1;35m"
NOTE_COLOR_CYAN_BOLD="\e[1;36m"

##
# Print
##
function notes_print()
{
    echo -e "$1"
}
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

##
# output HTML
##

#@param 1 The notes file
#@param 2 The name of output variable.
function __notes_view_generate_tmp_html_filename()
{
    local result=$2;
    # Sanitize the name for usage in temp dir.
    local TMP_OUTPUT_FILE_NAME=${1/.\/}
    TMP_OUTPUT_FILE_NAME=${TMP_OUTPUT_FILE_NAME//\//-}
    TMP_OUTPUT_FILE_NAME=${TMP_OUTPUT_FILE_NAME//.}

    TMP_OUTPUT_FILE_NAME="${TEMP_DIR}/${TMP_OUTPUT_FILE_NAME}.html"

    eval $result="'$TMP_OUTPUT_FILE_NAME'" 
}

# @param 1 The note file
# @param 2 The temp html file
function __notes_view_generate_html()
{
    $ASCIIDOC -o "$2" "$1"
}

function notes_view()
{
    local filename
    __notes_view_generate_tmp_html_filename $1 filename
    __notes_view_generate_html $1 $filename
    xdg-open $filename
}

##
# Editing
##

function __notes_edit_tmp_filename()
{
    local result=$2;
    # Sanitize the name for usage in temp dir.
    local TMP_OUTPUT_FILE_NAME=${1/.\/}
    TMP_OUTPUT_FILE_NAME=${TMP_OUTPUT_FILE_NAME//\//-}
    TMP_OUTPUT_FILE_NAME=${TMP_OUTPUT_FILE_NAME//.}

    TMP_OUTPUT_FILE_NAME="${TEMP_DIR}/${TMP_OUTPUT_FILE_NAME}.txt"

    eval $result="'$TMP_OUTPUT_FILE_NAME'" 
}

function notes_edit()
{
    local file;
    __notes_edit_tmp_filename "$1" file
    notes_info  "Editing file: $1"
    # copy the file
    cp "$1" "$file"

    # Update the edit part in header.
    TIMESTAMP=$(date) 

    # Take a checksum of the file before editing.
    MD5SUM_BEFORE=$(md5sum "$file")

    # Editing the file
    $EDITOR "$file"

    # Take a checksum of the file after editing.
    MD5SUM_AFTER=$(md5sum "$file")
   
    sed -i "s|:Date:.*|:Date: $TIMESTAMP|g" "$file"
    # Compare the checksum 
    if [ ! "$MD5SUM_BEFORE" = "$MD5SUM_AFTER" ]
    then
        notes_info "File has been changed, updating"
        mv "$file" "$1"
        notes_vcs_add_changes "$1"
    else
        notes_info "File is unchanged"
        rm "$file"
    fi
}

#@param 1 id
#@param 2 output of filename
function notes_get_from_id()
{
    local note_file
    local result=$2;
    note_file=$(find -type f -iname "$1.txt")
    if [ -z "$note_file" ]
    then
        notes_error "Failed to find note: $1"
        exit 1;
    fi
    eval $result="'$note_file'"
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
}

function __notes_category_name()
{
    local result=$2
    local __category=$(dirname "$1")
    __category=${__category/.\/}
    __category=${__category//\//.}
    eval $result="'$__category'" 
}

################
# List
################
function notes_list()
{
    local NOTES_FILES=$(find -type f -iname [0-9].txt | sort -n )
    # Get max category length
    declare -i max_cat_length=0;
    declare -i id_length=0;
    for file in $NOTES_FILES
    do
        local id=$(basename $file ".txt")
        local category;
        __notes_category_name "$file" category
        local length=${#category}
        if [ $length -gt $max_cat_length ]; then max_cat_length=$((length)); fi
        length=${#id}
        if [ $length -gt $id_length ]; then id_length=$length; fi
    done
    for file in $NOTES_FILES
    do
        declare -i id
        local category;
        __notes_category_name "$file" category
        declare -i show=1;
        if [ -n "$1" ]
        then
            show=0;
            # Check content of file.
            egrep -q -i -e "$1" "$file" 
            local in_content=$?
            if [ $in_content == 0 ] || [[ "${category}" =~ .*"$1".* ]]
            then
                show=1;
            fi
        fi
        if [ $show == 1 ]
        then
            id=$(basename $file ".txt")
            name=$(notes_get_name "$file")
            format=" %-${id_length}s "
            format+="${NOTE_COLOR_WHITE_BOLD}%-${max_cat_length}s${NOTE_COLOR_RESET} "
            format+=" %s\n"
            printf "$format" "$id" "$category" "$name"
        fi
    done
}


#########
# Adding
#########
function notes_add()
{
    local PROJECT=${1//./\/}
    local NOTES_FILES=($(find -type f -iname [0-9].txt -printf "%f\n" | sort -n))
    
    declare -i new_id=1;
    for file in "${NOTES_FILES[@]}"
    do
        local id=${file/.txt}
        echo "$new_id-$id"
        if [ $new_id -lt $id ]
        then
            break;
        fi
        new_id+=1;
    done
    # Create the filename
    file="./$PROJECT/$new_id.txt"
    local temp_file;
    __notes_edit_tmp_filename "$file" temp_file
    # Pipe in template:
    echo -e "{My Title}\n==============\n:Date: $(date)\n:Author: My name\n\ntext" > "$temp_file"

    # Take a checksum of the file before editing.
    local MD5SUM_BEFORE=$(md5sum "$temp_file")

    # Editing the file
    $EDITOR "$temp_file"

    # Take a checksum of the file after editing.
    MD5SUM_AFTER=$(md5sum "$temp_file")
   
    # Compare the checksum 
    if [ ! "$MD5SUM_BEFORE" = "$MD5SUM_AFTER" ]
    then
        notes_info "File has been changed, updating"
        notes_check_directory $(dirname $file)
        cp "$temp_file" "$file"
        notes_vcs_add_changes "$file"
        # TODO: commit
    else
        notes_info "File is unchanged"
    fi
}

################
# Delete
################
function notes_delete()
{
    notes_get_name $1
    notes_print "Are you sure you want to delete this note:"
    echo -n "(y/n): "
    answer="n"
    read answer
    if [ $answer == "y" ]
    then
        notes_print "Removing task:"
        notes_vcs_remove_file $1
    fi 
}
