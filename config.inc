PROGRAM_DIR=~/.local/share/notes/
INCLUDE_DIR=${PROGRAM_DIR}/includes/
GIT=$(which git)
NOTE_DIR=~/Notes/
TEMP_DIR=~/Notes/.temp/
ASCIIDOC=asciidoc
EDITOR=vim

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
