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
    echo "NetworkManager add bridge type connection"
    echo "Usage: nmcli-add-bridge-connection.sh <bridge device name> <slave device name (ex: enp5s0)>"
}

if [ "$2" = "" ]; then
    usage
    exit 1
fi

device_name="$1"
slave_device_name="$2"
connection_name="$device_name"
slave_connection_name="bridge-slave-$device_name-$slave_device_name"

echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo "NetworkManager add bridge connection type. coonection name: $connection_name, interfacename: $device_name, slave $slave_device_name"
echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo ""

#nmcli connection add type bridge con-name bridge-br0 ifname br0
nmcli connection add type bridge con-name "$connection_name" connection.interface-name "$device_name"
nmcli connection add type bridge-slave con-name "$slave_connection_name" connection.interface-name "$slave_device_name" master "$device_name"

# STP no
nmcli connection modify "$connection_name" bridge.stp no

#
# next you should
# set ip dns dnssearch gateway
# run nmcli connection up $connection_name
# run to verify
#     nmcli connection show
#     nmcli connection show $connection_name
#     ip addr show
# at the end to activate slave connection and active $connection_name for traffic
# nmcli connection delete "connection name for slave device" (usually same name as $slave_device_name)
#

