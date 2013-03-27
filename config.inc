PROGRAM_DIR=~/.local/share/notes/
INCLUDE_DIR=${PROGRAM_DIR}/includes/
GIT=$(which git)
NOTE_DIR=~/Notes/
TEMP_DIR=~/Notes/.temp/
ASCIIDOC=asciidoc
EDITOR=vim

if nm-online
then
    nm-tool | grep -e "^[\t ]Address" | grep -e "(131.155|192.150)"
    if [ $? == 0 ]
    then
        HAS_INET=1
    fi
fi
