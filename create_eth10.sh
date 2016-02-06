#!/usr/bin/env bash

modprobe dummy
lsmod | grep dummy
ip link set name eth10 dev dummy0
ip link show eth10
ifconfig eth10 hw ether 00:22:22:ff:ff:ff
ip link show eth10
ip addr add 192.168.100.199/24 brd + dev eth10 label eth10:0
ifconfig eth10 up
ifconfig eth10 promisc
