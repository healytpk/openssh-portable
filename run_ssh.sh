#!/bin/sh

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

./ssh --vpn -D 5555 dh_p7rcrw@virjacode.com /home/dh_p7rcrw/progs/badvpn-udpgw --listen-addr 127.0.0.1:7300
