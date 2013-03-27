PROGRAM_DIR=~/.local/share/notes/
INCLUDE_DIR=${PROGRAM_DIR}/includes/
GIT=$(which git)
NOTE_DIR=~/Notes/
TEMP_DIR=~/Notes/.temp/
ASCIIDOC=asciidoc
EDITOR=vim

if nm-online
then
    HAS_INET=1
fi
