function get_note_name()
{
    HEADING=$(head -n 1 "$1")
    echo ${HEADING}
}
