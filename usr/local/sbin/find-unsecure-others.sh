#!/bin/bash

#
# Released under MIT License
# Copyright (c) 2018-2025 Jose Manuel Churro Carvalho
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
# and associated documentation files (the "Software"), to deal in the Software without restriction, 
# including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
# and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

usage()
{
    echo "find unsecure files (others perms) that can contain danger words or data"
    echo "Now search for .conf .cfg .properties .xml .log .txt .rb .yml files"
    echo "Search for keywords like password, passwd, pwd, credentials, secret, pass"
    echo "To improve ... just add keywords and file extensions ..."
    echo "Usage: find-unsecure-others.sh <path> ..."
}

if [ "$1" = "" ]; then
    usage
    exit 1
fi

#

i=0

for arg in "$@"; do
    echo ">> Path: $arg"

    find -L "$arg" -type f \
        \( -iname "*.xml" -o -iname "*.conf*" -o -iname "*.properties*" -o -iname "*.cfg*" \
            -o -iname "*.log" -o -iname "*.txt" \
            -o -iname "*.rb" -o -iname "*.yml" -o -iname "*.yaml" \
            -o -name "*.service" \) \
        -perm /o=rwx \
        -exec grep -i --with-filename -e "(password)|(pwd)|(passwd)|(creden)|(pass)|(secret)|(login)" '{}' \;
    i=$((i+1))
done

exit 0

