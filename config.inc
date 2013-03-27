# Location of the include files.
PROGRAM_DIR=~/.local/share/notes/
INCLUDE_DIR=${PROGRAM_DIR}/includes/

# Location of the notes.
NOTE_DIR=~/Notes/

# Tools that are used.
GIT=$(which git)
ASCIIDOC=$(which asciidoc)
EDITOR=$(which vim)

# Temporary directory
TEMP_DIR=${NOTE_DIR}/.temp/

# check online status
NM_TOOL=$(which nm-online)

if [ -n "${NM_TOOL}" ]
then
    if ${NM_TOOL}
    then
        nm-tool | grep -e "^[\t ]Address" | grep -e "(131.155|192.150)"
        if [ $? == 0 ]
        then
            HAS_INET=1
        fi
    fi
else
    HAS_INET=1
fi
