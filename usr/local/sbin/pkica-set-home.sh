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
    echo "Set CA home dir for pkica"
    echo "Usage: pkica-set-home.sh <suffix string or 0 for production. Use ca-qua to testing for instance>"
}

if [ "$1" = "" ]; then
    usage
    exit 1
fi

PKICA_CA_HOME_BASE=/usr/local/etc/pki/ca
export PKICA_CA_HOME_BASE

if [ "$1" = "0" ]; then
    PKICA_CA_HOME="$PKICA_CA_HOME_BASE"/"ca"
else
    PKICA_CA_HOME="$PKICA_CA_HOME_BASE"/"$1"
fi
export PKICA_CA_HOME

