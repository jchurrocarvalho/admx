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
    echo "ipa getcert request for HTTP Virtual Host"
    echo "Usage: ipa-http-service-cert-request <virtual host> <hostname (managed by)>"
    echo "Remember to create the DNS entry first"
}

if [ "$2" = "" ]; then
    usage
    exit 2
fi

echo -n "Have you already created the DNS entry? (Press [y] to continue or any key else to stop): "
read -r dns_entry_already_created

if [ "$dns_entry_already_created" = "y" ] || [ "$dns_entry_already_created" = "Y" ]; then
    ipa host-add "$1"
    retvalue=$?
    if [ "$retvalue" != "0" ]; then
        echo "An error was returned. {Line: $LINENO, Error Code: $retvalue}"
        exit $retvalue
    fi

    ipa host-add-managedby "$1" --hosts="$2"
    retvalue=$?
    if [ "$retvalue" != "0" ]; then
        echo "An error was returned. {Line: $LINENO, Error Code: $retvalue}"
        exit $retvalue
    fi

    ipa service-add HTTP/"$1"
    retvalue=$?
    if [ "$retvalue" != "0" ]; then
        echo "An error was returned. {Line: $LINENO, Error Code: $retvalue}"
        exit $retvalue
    fi

    ipa service-add-host HTTP/"$1" --hosts="$2"
    retvalue=$?
    if [ "$retvalue" != "0" ]; then
        echo "An error was returned. {Line: $LINENO, Error Code: $retvalue}"
        exit $retvalue
    fi

    ipa-getcert request -r -f /etc/pki/tls/certs/http_"$1".crt -k /etc/pki/tls/private/http_"$1".key -N CN="$1" -D "$1" -K HTTP/"$1"
    retvalue=$?
    if [ "$retvalue" != "0" ]; then
        echo "An error was returned. {Line: $LINENO, Error Code: $retvalue}"
        exit $retvalue
    fi

    exit 0
else
    exit 2
fi

