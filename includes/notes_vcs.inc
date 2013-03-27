function notes_vcs_check_updates()
{
    # Check for changes.
    $GIT diff-files --quiet
    if [ $? == 1 ]
    then
        notes_info "Stashing changes"
        $GIT stash --quiet
        if [ $? == 1 ]; then notes_error "Failed to stash the changes, please fix manually."; exit 1; fi
        notes_info "Pulling"
        $GIT pull --quiet
        if [ $? == 1 ]; then notes_error "Failed to pull the changes, please fix manually."; exit 1; fi
        notes_info "Applying changes"
        $GIT stash pop --quiet
        if [ $? == 1 ]; then notes_error "Failed to apply the changes, please fix manually."; exit 1; fi
    fi
}

#param 1 the file that changed
function notes_vcs_add_changes()
{
    $GIT add --quiet "$1"
    if [ $? == 1 ]
    then 
        notes_error "Failed to add file: %1.";
        exit 1; 
    fi
}


function notes_vcs_commit_changes()
{
    # check for changes.
    $GIT diff-files --quiet
    if [ $? == 1 ]
    then
        notes_info "Commiting changes"
        DATE=$(date)
        $GIT commit --quiet -m "Auto updated on $DATE"
        if [ $? == 1 ]
        then
            notes_error "Failed to commit changes";
            exit 1;
        fi
    fi
}

function notes_vcs_push_changes()
{
    $GIT push --quiet
    if [ $? == 1 ]
    then
        notes_error "Pushing to remote server"
        exit 1;
    fi
}


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
