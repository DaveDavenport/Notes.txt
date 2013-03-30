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


# @param 1 the input array
# @param 2 the output array
function __sort_notes_ids()
{
    local __sort_result=$2
    local __sort_ARRAY=( ${1} )
    declare -i count=${#__sort_ARRAY[@]}
    declare -i last=0;

    for (( last=count-1;last>0;last--))
    do
        for((i=0;i<last;i++))
        do
          j=$((i+1))

          local val_a;
          local val_b;

          val_a=$(basename "${__sort_ARRAY[i]}" ".txt")
          val_b=$(basename "${__sort_ARRAY[j]}" ".txt")

          if [ "$val_a" -gt "$val_b" ] 
          then
            # Swap elements around.
            temp="${__sort_ARRAY[i]}"
            __sort_ARRAY[i]="${__sort_ARRAY[j]}"
            __sort_ARRAY[j]="${temp}"
          fi
        done
    done
    eval $__sort_result="'${__sort_ARRAY[@]}'"
}


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


function notes_validate_category()
{
    regex="^[0-9a-zA-Z.]*$"
    if [[ $1 =~ $regex ]] 
    then
        return;
    else
        notes_error "Category is invalid: $1"
        exit 1
    fi
}

function notes_get_name()
{
    HEADING=$(${NOTES_GET_DESCRIPTION} "$1")
    echo ${HEADING}
}


function notes_get_date()
{
    local note_file="$1";
    local __result=$2;
    __date=$(eval ${NOTES_GET_DATE});
    eval $__result="'${__date}'" 
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
    eval ${EDITOR} "$file"

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
        # Do content based matching
        local NOTES_FILES=$(find -type f -iname [0-9].txt)

        __sort_notes_ids "${NOTES_FILES[@]}" NOTES_FILES 

        local __result;
        __internal_filter_list "${NOTES_FILES[@]}" "$1" __result 
        declare -i count="${#__result[@]}"

        if [ $count == 1 ]
        then
            eval $result="'${__result[0]}'"
            return; 
        else
            notes_error "Found ${#__result[@]} possible matches."
            __internal_notes_list "${__result[@]}"
            exit 1;
        fi

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

function __internal_filter_list()
{
    local __filtert=$2
    local __filter_result=$3
    local __filter_result_array=( )
    local __ARRAY=( ${1} )
    declare -i count=0;

    for file in ${__ARRAY[@]}
    do
        declare -i id
        local category;
        __notes_category_name "$file" category
        declare -i show=1;
        if [ -n "$__filtert" ]
        then
            show=0;
            # Check content of file.
            egrep -q -i -e "$__filtert" "$file" 
            local in_content=$?
            if [ $in_content == 0 ] || [[ "${category}" =~ .*"$__filtert".* ]]
            then
                show=1;
            fi
        fi
        if [ $show == 1 ]
        then
            __filter_result_array[count]="${file}"
            count+=1;
        fi
    done

    eval $__filter_result='( ${__filter_result_array[@]} )'
}

function __internal_notes_list()
{
    local NOTES_FILES=( ${@} )

    # Get max category length
    declare -i max_cat_length=7;
    declare -i id_length=2;
    declare -i desc_length=11;
    declare -i date_length=11;

    for file in ${NOTES_FILES[@]}
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

        notes_get_date "${file}" id
        id=$(date -d "${id}" "+${DATE_FORMAT}")
        length=${#id}
        if [ $length -gt $date_length ]; then date_length=$length; fi
    done

    # Generate header
    printf "\n"
    printf "${NOTE_BOLD}${NOTE_UNDERLINE}%-${id_length}s${NOTE_RESET}  " "ID" 
    printf "${NOTE_BOLD}${NOTE_UNDERLINE}%-${max_cat_length}s${NOTE_RESET}  " "Project"
    printf "${NOTE_BOLD}${NOTE_UNDERLINE}%-${date_length}s${NOTE_RESET}  "  "Last edited"
    printf "${NOTE_BOLD}${NOTE_UNDERLINE}%-${desc_length}s${NOTE_RESET}  "  "Description"
    printf "\n"

    # Print hits
    for file in ${NOTES_FILES[@]}
    do
        declare -i id
        local category;
        __notes_category_name "$file" category

        local date=""
        id=$(basename $file ".txt")
        name=$(notes_get_name "$file")

        notes_get_date "${file}" date 
        date=$(date -d "${date}" "+${DATE_FORMAT}")

        format="${NOTE_COLOR_YELLOW_BOLD}%-${id_length}s${NOTE_RESET}  "
        format+="${NOTE_COLOR_WHITE_BOLD}%-${max_cat_length}s${NOTE_RESET}  "
        format+="${NOTE_COLOR_GREEN_BOLD}%-${date_length}s${NOTE_RESET}  "
        format+="%s\n"
        printf "$format" "$id" "$category" "${date}" "$name"
    done
}

function notes_list()
{
    local NOTES_FILES=$(find -type f -iname [0-9].txt)

    # Sort the notes
    __sort_notes_ids "${NOTES_FILES[@]}" NOTES_FILES

    # filter list
    __internal_filter_list "${NOTES_FILES[@]}" "$1" NOTES_FILES

    # show list
    __internal_notes_list "${NOTES_FILES[@]}"
}


#########
# Adding
#########
function notes_add()
{
    notes_validate_category "${1}"

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
    eval ${EDITOR} "$temp_file"

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



