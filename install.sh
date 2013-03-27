#!/bin/bash


if [ ! -f ~/.notes.config ]
then
    ln -s  $PWD/config.inc ~/.notes.config
fi

if [ ! -d  ~/.local/share/notes/ ]
then
    mkdir -p ~/.local/share/notes/
fi

if [ ! -L ~/.local/share/notes/includes ]
then
    ln -s  $PWD/includes ~/.local/share/notes/includes
fi
if [ ! -x ~/.local/bin/notes.sh ]
then
    ln -s $PWD/notes.sh ~/.local/bin/notes.sh
fi

grep "notes.autocomplete" ~/.bash_functions -q
if [ $? == 1 ]
then
    echo ". $PWD/notes.autocomplete" >> ~/.bash_functions
fi 
