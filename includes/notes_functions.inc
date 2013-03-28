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

# Include the VCS stuff
. ${INCLUDE_DIR}/notes_vcs.inc

# List of colors.
. ${INCLUDE_DIR}/notes_colors.inc

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
    echo -e $NOTE_COLOR_BLUE"$1"$NOTE_RESET >&1
}

##
# Print an error message
##
function notes_error()
{
    echo -e $NOTE_COLOR_RED_BOLD"$1"$NOTE_RESET >&2
}


function notes_get_name()
{
    HEADING=$(${NOTES_GET_DESCRIPTION} "$1")
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
    $MARKUP_PARSER "$1" > "$2"
    if [ $? != 0 ]
    then
        notes_error "Failed to create html file"
        exit 1;
    fi
}

function notes_view()
{
    local temp_file;
    __notes_edit_tmp_filename "$1" temp_file
    local filename
    if [ -f "${temp_file}" ]
    then
        __notes_view_generate_tmp_html_filename "${1}" filename
        __notes_view_generate_html "${temp_file}" $filename
    else
        __notes_view_generate_tmp_html_filename "$1" filename
        __notes_view_generate_html "$1" $filename
    fi
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

    if [ -f "${file}" ]
    then
        notes_error "This note is already being edited"
        exit 1;
    fi

    # copy the file
    cp "$1" "$file"

    # Update the edit part in header.
    TIMESTAMP="$(date)  (Editing..)"
    eval ${NOTES_MODIFY_DATE} 

    # Take a checksum of the file before editing.
    MD5SUM_BEFORE=$(md5sum "$file")

    # Editing the file
    $EDITOR "$file"

    # Take a checksum of the file after editing.
    TIMESTAMP=$(date) 
    MD5SUM_AFTER=$(md5sum "$file")
   
    eval ${NOTES_MODIFY_DATE} 

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
    declare -i max_cat_length=7;
    declare -i id_length=2;
    declare -i desc_length=11;
    for file in $NOTES_FILES
    do
        local id=$(basename $file ".txt")
        local category;
        __notes_category_name "$file" category
        local length=${#category}
        if [ $length -gt $max_cat_length ]; then max_cat_length=$((length)); fi
        length=${#id}
        if [ $length -gt $id_length ]; then id_length=$length; fi
        name=$(notes_get_name "$file")
        length=${#name}
        if [ $length -gt $desc_length ]; then desc_length=$length; fi
    done

    # Generate header
    printf "\n"
    printf "${NOTE_UNDERLINE}%-${id_length}s${NOTE_RESET} " "ID" 
    printf "${NOTE_UNDERLINE}%-${max_cat_length}s${NOTE_RESET} " "Project"
    printf "${NOTE_UNDERLINE}%-${desc_length}s${NOTE_RESET}\n"  "Description"

    # Print hits
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
            format="${NOTE_COLOR_YELLOW_BOLD}%-${id_length}s${NOTE_RESET} "
            format+="${NOTE_COLOR_WHITE_BOLD}%-${max_cat_length}s${NOTE_RESET} "
            format+="%s\n"
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
    echo -e "${NOTES_TEMPLATE}" > "$temp_file"

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
    local answer="n"
    read answer
    if [ $answer == "y" ]
    then
        notes_print "Removing task:"
        notes_vcs_remove_file $1
    fi 
}
