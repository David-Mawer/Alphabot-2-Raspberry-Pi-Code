#!/bin/bash
# This probably required when development's going to be done..
# so - sort out XTERM fonts - since I cen't seem to get that fixed on boot
xrdb -remove
xrdb -merge /home/pi/.Xdefaults

clear
echo "The following may contain a psid for the blueBot service: -"
echo ""
echo "-------------------------------------------------------------------"
#ps aux | grep 'sudo rfcomm'
ps aux | grep DadBot
echo "-------------------------------------------------------------------"
echo ""
echo "If there's a row that starts with root - then that is probably the service"
echo ""
echo "type out the following command to kill it gracefully: -"
echo "    sudo kill -TERM XXX"
echo ""
