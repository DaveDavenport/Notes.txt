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
# Pull the latest changes from remote repository.
##
function notes_vcs_check_updates()
{
    # Check for changes.
    $GIT diff-index HEAD --quiet
    if [ $? == 1 ]
    then
        notes_info "Stashing changes"
        $GIT stash save --quiet
        if [ $? == 1 ]; then notes_error "Failed to stash the changes, please fix manually."; exit 1; fi
        notes_info "Pulling"
        $GIT pull --quiet
        if [ $? == 1 ]; then notes_error "Failed to pull the changes, please fix manually."; exit 1; fi
        notes_info "Applying changes"
        $GIT stash pop --quiet
        if [ $? == 1 ]; then notes_error "Failed to apply the changes, please fix manually."; exit 1; fi
     else
        $GIT pull --quiet
        if [ $? == 1 ]; then notes_error "Failed to pull the changes, please fix manually."; exit 1; fi
     fi
}

##
# @param 1 the file that changed
#
# Stage the changed file.
##
function notes_vcs_add_changes()
{
    $GIT add "$1"
    if [ $? == 1 ]
    then 
        notes_error "Failed to add file: %1.";
        exit 1; 
    fi
}

##
# Commit the staged changed
##
function notes_vcs_commit_changes()
{
    # check for changes.
    $GIT diff-index HEAD --quiet
    if [ $? == 1 ]
    then
        notes_info "Commiting changes"
        DATE=$(date)
        $GIT commit -m "Auto updated on $DATE"
        if [ $? == 1 ]
        then
            notes_error "Failed to commit changes";
            exit 1;
        fi
    fi
}

##
# Push the local commits to the remote location.
##
function notes_vcs_push_changes()
{
    PARENT_BRANCH=$($GIT rev-parse --abbrev-ref --symbolic-full-name @{u})
    $GIT diff-index --quiet "${PARENT_BRANCH}" --
    if [ $? == 1 ]
    then
        notes_info "Pushing changes"
        $GIT push --quiet
        if [ $? == 1 ]
        then
            notes_error "Pushing to remote server"
            exit 1;
        fi
    fi
}

##
# @param directory
# Check if the specified directory is a toplevel git repository
##
function notes_vcs_validate_dir()
{
    TOPLEVEL_DIR=$($GIT rev-parse --show-toplevel)
    if [ $? != 0 ]
    then
        notes_error "Current directory is not a git repository"
        exit 1;
    fi

    if [[ "$(stat -L -c \"%d:%i\" $1)" != "$(stat -L -c \"%d:%i\" $TOPLEVEL_DIR)" ]]
    then
        notes_error "Current note directory: $1 is not the toplevel Git repository.\nToplevel directory is: $TOPLEVEL_DIR"
        exit 1;
    fi
}

##
# @param the file to remove
# Stage removal of file.
##
function notes_vcs_remove_file()
{
    $GIT rm $1
    if [ $? != 0 ]
    then
        notes_error "Failed to remove file from repo: $1"
        exit 1;
    fi
}
