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

##
# Load notes into global list.
##
function notes_global_load_list()
{
    declare -i index=1; 
    local __gl_nl=$(find -type f -iname [0-9]*txt | awk -F/ '{print $NF"|"$0}' | sort -n | cut -d"|" -f2)

    for file in ${__gl_nl}
    do
        GLOBAL_NOTES_LIST[$index]="${file}"
        index+=1;
        GLOBAL_NUM_NOTES+=1;
    done

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

function notes_get_revision()
{
    local note_file=$1;
    local __ngr_result=$2;
    REVISION=$(eval ${NOTES_GET_REVISION})

    eval $__ngr_result="'${REVISION}'"
}

function notes_get_name()
{
    local __ngn_result=$2;
    HEADING=$(${NOTES_GET_DESCRIPTION} "$1")
    eval $__ngn_result="'${HEADING}'"
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
function __notes_view_generate_view_format()
{
    local output_format="${MARKUP_VIEW_FORMAT}"
    eval $MARKUP_PARSER "$1" > "$2"
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
        __notes_view_generate_view_format "${temp_file}" $filename
    else
        __notes_view_generate_tmp_html_filename "$1" filename
        __notes_view_generate_view_format "$1" $filename
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

    # Update revision number.
    declare -i cur_rev=1;
    # Get the current revision.
    notes_get_revision "$1" cur_rev;
    # Calculate next revision
    declare -i next_rev=${cur_rev}+1;
    # Update.
    eval ${NOTES_ADD_REVISION}

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
    local __ngfi_note_file
    local result=$2;
    if [[ $1 =~ ^[0-9]+$ ]]
    then
        __ngfi_note_file=${GLOBAL_NOTES_LIST["$1"]}
    fi
    if [ -z "$__ngfi_note_file" ]
    then
        # Do content based matching
        local __result;
        __internal_filter_list "$1";
        declare -i count=${GLOBAL_NUM_NOTES}

        if [ $count == 1 ]
        then
            eval $result="'${GLOBAL_NOTES_LIST[@]}'"
            return; 
        else
            notes_error "Found ${#__result[@]} possible matches."
            __internal_notes_list "${__result[@]}"
            id="" 
            while [ -z "$id" ]
            do
                notes_print "Please enter note id: (q is quit)"
                read id;
                if [ "$id" == "q" ]
                then
                    popd > /dev/null
                    exit 0;
                fi
            done
            __ngfi_note_file=$(find -type f -iname "$id.txt")
            if [ -n "$__ngfi_note_file" ]
            then
                eval $result="'$__ngfi_note_file'"
                return;
            else
                notes_error "Failed to find note with id: $id"
            fi
            exit 1;
        fi
    fi
    eval $result="'$__ngfi_note_file'"
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
    local __filtert=$1
    declare -i count=0;

    for index in $(seq 1 ${GLOBAL_NUM_NOTES}) 
    do
        declare -i id
        local category;
        local file=${GLOBAL_NOTES_LIST[$index]}
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
        if [ $show == 0 ]
        then
            unset GLOBAL_NOTES_LIST[$index]
            GLOBAL_NUM_NOTES=${GLOBAL_NUM_NOTES}-1;
        fi
    done
}

function __internal_notes_list()
{
    # Get max category length
    declare -i max_cat_length=7;
    declare -i id_length=2;
    declare -i desc_length=11;
    declare -i date_length=11;

    declare -i revision_length=3;

    # Keep data from first run around. 
    # this saves the time of fetching it again.
    declare -a __dates;
    declare -a __names;
    declare -a __categories;
    declare -a __revisions;

    # This function is the slowest.
    # Optimizing this would be good.
    for index in ${!GLOBAL_NOTES_LIST[@]}
    do
        local file=${GLOBAL_NOTES_LIST[index]}
        # ID.
        local id=$index;
        length=${#id}
        if [ $length -gt $id_length ]; then id_length=$length; fi

        # Category
        local category;
        __notes_category_name "$file" category
        local length=${#category}
        if [ $length -gt $max_cat_length ]; then max_cat_length=$((length)); fi
        __categories[$id]="${category}"

        # Name
        local name
        notes_get_name "$file" name
        length=${#name}
        if [ $length -gt $desc_length ]; then desc_length=$length; fi
        __names[$id]="$name"

        # Date
        local mdate;
        notes_get_date "${file}" mdate
        mdate=$(date -d "${mdate}" "+${DATE_FORMAT}")
        length=${#mdate}
        if [ $length -gt $date_length ]; then date_length=$length; fi
        __dates[$id]="$mdate"

        # Revision
        local nrev;
        notes_get_revision "${file}" nrev
        length=${#nrev}
        if [ $length -gt $revision_length ]; then revision_length=$length; fi
        __revisions[$id]="$nrev"
    done

    notes_timestamp "__internal_notes_list(): Calculate lengths"
    # Generate header
    printf "\n"
    printf "${NOTE_BOLD}${NOTE_UNDERLINE}%-${id_length}s${NOTE_RESET}  " "ID" 
    printf "${NOTE_BOLD}${NOTE_UNDERLINE}%-${revision_length}s${NOTE_RESET}  "  "Rev"
    printf "${NOTE_BOLD}${NOTE_UNDERLINE}%-${max_cat_length}s${NOTE_RESET}  " "Project"
    printf "${NOTE_BOLD}${NOTE_UNDERLINE}%-${date_length}s${NOTE_RESET}  "  "Last edited"
    printf "${NOTE_BOLD}${NOTE_UNDERLINE}%-${desc_length}s${NOTE_RESET}  "  "Description"
    printf "\n"

    notes_timestamp "__internal_notes_list(): Headers"
    # Print hits
    for index in ${!GLOBAL_NOTES_LIST[@]}
    do
        local file=${GLOBAL_NOTES_LIST[index]}
        declare -i id=$index

        local date=""

        format="${NOTE_COLOR_YELLOW_BOLD}%${id_length}s${NOTE_RESET}  "
        format+="${NOTE_COLOR_BLUE}%${revision_length}s${NOTE_RESET}  "
        format+="${NOTE_COLOR_WHITE_BOLD}%-${max_cat_length}s${NOTE_RESET}  "
        format+="${NOTE_COLOR_GREEN_BOLD}%-${date_length}s${NOTE_RESET}  "
        format+="%s\n"
        printf "$format" "$id" "${__revisions[$id]}" "${__categories[$id]}" "${__dates[$id]}" "${__names[$id]}"
    done
    notes_timestamp "__internal_notes_list(): Show rows"
}

function notes_list()
{
    notes_timestamp "notes_list(): Get list"
    # filter list
    if [ -n "$1" ]
    then
        notes_print "${NOTE_COLOR_BLUE}Filtered by:${NOTE_RESET}  $1"
    fi
    __internal_filter_list "$1" 
    notes_timestamp "notes_list(): Filter list"

    # show list
    __internal_notes_list;
    notes_timestamp "notes_list(): Show list"
}


#########
# Adding
#########
function notes_add()
{
    notes_validate_category "${1}"

    local PROJECT=${1//./\/}

    notes_info "Looking for free id:"
    declare -i new_id=$(date +%s);

    notes_info "Found id: ${new_id}"
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
    local name;
    notes_get_name $1 name
    notes_print "${name}"
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



