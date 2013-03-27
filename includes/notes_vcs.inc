function notes_vcs_check_updates()
{
    $GIT diff-files --quiet
    if [ $? == 1 ]
    then
        notes_info "Stashing changes"
        $GIT stash
        if [ $? == 1 ]; then notes_error "Failed to stash the changes, please fix manually."; exit 1; fi
        notes_info "Pulling"
        $GIT pull
        if [ $? == 1 ]; then notes_error "Failed to pull the changes, please fix manually."; exit 1; fi
        notes_info "Applying changes"
        $GIT stash pop
        if [ $? == 1 ]; then notes_error "Failed to apply the changes, please fix manually."; exit 1; fi
    fi
}
