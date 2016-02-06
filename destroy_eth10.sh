#!/usr/bin/env bash

ip addr del 192.168.100.199/24 brd + dev eth10 label eth10:0
ip link delete eth10 type dummy
rmmod dummy
