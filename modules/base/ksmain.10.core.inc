lang en_US.UTF-8
keyboard us
timezone --utc GMT
auth --useshadow --enablemd5
selinux --disabled
firewall --disabled
xconfig --startxonboot
part / --size 2048 --fstype=ext3
services --enabled=messagebus,NetworkManager,avahi-daemon,crond --disabled=dnsmasq,ip6tables,iptables,mdmonitor,netfs,network

