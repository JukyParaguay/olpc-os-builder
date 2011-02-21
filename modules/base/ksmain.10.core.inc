lang en_US.UTF-8
keyboard us
auth --useshadow --enablemd5
selinux --disabled
firewall --disabled
xconfig --startxonboot
part / --size 4096 --fstype=ext3
services --enabled=messagebus,NetworkManager,avahi-daemon,crond --disabled=dnsmasq,ip6tables,iptables,mdmonitor,netfs,network

