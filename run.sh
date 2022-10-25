#! /bin/bash
clear
if [[ -f craftinginterpreters ]]; then
    rm craftinginterpreters
fi
echo building
echo
go build

./craftinginterpreters $@
