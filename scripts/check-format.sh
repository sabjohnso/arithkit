#!/bin/bash
set -eu

readonly scriptpath=$0;
readonly scriptdir=$(dirname $0)
readonly srcdir=$scriptdir/../src
readonly testdir=$scriptdir/../test

badfile_count=0

function main(){

    check_files $srcdir
    check_files $testdir
}

function check_file_format(){
    local inpfile=$1; shift
    local tmpfile=$(mktemp)

    clang-format --style=file $inpfile > $tmpfile
    if [[ $(diff -q $inpfile $tmpfile) ]]
    then
        echo "needs to be formatted: $inpfile"
        badfile_count+=1
    fi
    rm $tmpfile
}

function check_files(){
    local inpdir=$1; shift
    local source_files=$(find $inpdir -type f -name "*.hpp")
    for source_file in $source_files
    do
        if [[ -f "$source_file" ]]
        then
            check_file_format $source_file
        fi
    done
}

main
exit $badfile_count
