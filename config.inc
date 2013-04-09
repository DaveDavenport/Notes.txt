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


# Location of the include files.
PREFIX=~/.local/
PROGRAM_DIR=${PREFIX}/share/notes/
INCLUDE_DIR=${PROGRAM_DIR}/includes/

# Location of the notes.
NOTE_DIR=~/Notes/

# Tools that are used.
GIT=git

if [ -z "${EDITOR}" ]
then
EDITOR="gvim -c 'set syntax=asciidoc' -f "
fi

# Temporary directory
TEMP_DIR=${NOTE_DIR}/.temp/


#DO_AUTO_PUSH_PULL=1


# date format
DATE_FORMAT="%F"


##
# This configures what MARKUP tool to use.
##

# give the output format this backend supports.
MARKUP_OUTPUT_FORMATS=( "html" "docbook" "wordpress" )

# The default view format.
MARKUP_VIEW_FORMAT="html"

############################################
# ASCIIDOC
############################################

# The tool to parse input and cat to output.
MARKUP_PARSER="asciidoc -o - -b \${output_format}"

# This should extract the description string from the input file.
NOTES_GET_DESCRIPTION="sed 1q"

# This should modify the timestamp in the input file.
# \${TIMESTAMP} is the new timestamp, "\${file}"  is the file that will be edited.
NOTES_MODIFY_DATE="sed -i \"s|:Date:.*|:Date: \${TIMESTAMP}|g\" \"\$file\" "

# A templete (empty) markup file.
NOTES_TEMPLATE="{My Title}\n==============\n:Date: $(date)\n:Author: My name\n:Revision: 1.0, Initial note\n\ntext"

# Get the file from a note file. Input file is '${note_file}'
NOTES_GET_DATE="sed -n \"s|:Date: \(.*\)\$|\1|p\" \"\${note_file}\""

NOTES_GET_REVISION="sed -n \"s/:Revision: \([0-9.]*\).*/\1/p\" \"\${note_file}\" | sort -n -r | head -n 1"
NOTES_ADD_REVISION="sed -i \"/:Revision: \${cur_rev}/a:Revision: \${next_rev}, Reason of update\" \"\${file}\""

################################################
# Markup
################################################
#MARKUP_PARSER="markdown"
#NOTES_GET_DESCRIPTION=
#NOTES_MODIFY_DATE=
#NOTES_TEMPLATE=
