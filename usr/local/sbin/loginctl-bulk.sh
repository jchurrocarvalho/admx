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
    echo "loginctl machines"
    echo "Usage: loginctl-bulk.sh <host file list>"
}

if [ "$1" = "" ]; then
    usage
    exit 2
fi

hostname=$(eval 'hostname')

targethostname=""
port=""

i=0
retvalueend=0

INPUTFILENAME="$1"

#for line in $(cat "$INPUTFILENAME"); do
while IFS= read -r line
do
    if [ -n "$line" ]; then
        if [ "${line:0:1}" != "#" ]; then
            if [ "${line:0:1}" = "P" ]; then
                port=""
                i=1
                while [ $i -lt "${#line}" ] && [ "${line:$i:1}" != "H" ]; do
                    port+="${line:$i:1}"
                    i=$((i+1))
                done
                i=$((i+1))
                targethostname="${line:$i}"
            else
                port="22"
                targethostname="$line"
            fi
            if [ -n "$targethostname" ] && [ -n "$port" ] && [ "$hostname" != "$targethostname" ]; then
                echo "================================================================"
                echo "host: $targethostname:$port"
                echo "================================================================"
                ssh -n -p "$port" "$targethostname" loginctl
                retvalue=$?
                if [ "$retvalue" != "0" ]; then
                    echo "An error was returned. {Host: $targethostname, Port: $port}, {Line: $LINENO, Error Code: $retvalue}"
                    retvalueend=$retvalue
                fi
                echo ""
            fi
        fi
    fi
done < "$INPUTFILENAME"

exit $retvalueend

