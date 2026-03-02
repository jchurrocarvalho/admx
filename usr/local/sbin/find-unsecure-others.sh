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

SCRIPTTITLE="find-unsecure-others"

usage()
{
    echo "find unsecure files (others perms) that can contain danger words or data"
    echo "Now search for .conf .cfg .properties .xml .log .txt .rb .yml files"
    echo "Search for keywords like password, passwd, pwd, credentials, secret, pass"
    echo "To improve ... just add keywords and file extensions ..."
    echo "Usage: $SCRIPTTITLE.sh <send also to syslog? (0/1)> <path> ..."
}

if [ "$2" = "" ]; then
    usage
    exit 2
fi

#

if [ "$1" = "1" ]; then
    SENDSYSLOG="1"
else
    SENDSYSLOG="0"
fi

i=0
retvalue=0

if [ "$SENDSYSLOG" = "1" ]; then
    logger --tag "$SCRIPTTITLE" "INFO. $SCRIPTTITLE.sh has started."
fi
for arg in "$@"; do
    if [ $i -ge 1 ]; then
        echo ">> Path: $arg"

        if [ -d "$arg" ]; then
            echo ">>>> Find unsecure files (others perms) that can contain danger words or data ..."
            CMD='find -L "$arg" -type f \
                \( -iname "*.xml" -o -iname "*.conf*" -o -iname "*.properties*" -o -iname "*.cfg*" \
                    -o -iname "*.log" -o -iname "*.txt" \
                    -o -iname "*.rb" -o -iname "*.yml" -o -iname "*.yaml" \
                    -o -name "*.service" -o -name "*.*sh" \) \
                -perm /o=rwx \
                -exec grep -i --with-filename -e "(password)|(pwd)|(passwd)|(creden)|(pass)|(secret)|(login)" '{}' \;'
            if [ "$SENDSYSLOG" = "1" ]; then
                CMD="$CMD"" -exec logger --tag \"$SCRIPTTITLE\" \"WARN. unsecure file: \"{} \;"
            fi
            eval "$CMD"
            retvalue=$?
            if [ "$retvalue" != "0" ]; then
                echo "An error was returned. {Line: $LINENO, Error Code: $retvalue}"
                #break
            fi
        else
            echo ">>>> $arg directory does not exist."
        fi
    fi
    i=$((i+1))
done
if [ "$SENDSYSLOG" = "1" ]; then
    logger --tag "$SCRIPTTITLE" "INFO. $SCRIPTTITLE.sh has finshed."
fi

exit $retvalue

