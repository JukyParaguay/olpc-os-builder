lang en_US.UTF-8
keyboard us
timezone --utc GMT
auth --useshadow --enablemd5
selinux --disabled
firewall --disabled
xconfig --startxonboot
part / --size 2048 --fstype=ext3
services --enabled=messagebus,NetworkManager,avahi-daemon,crond --disabled=atd,auditd,cpuspeed,dnsmasq,ip6tables,iptables,irda,mdmonitor,netfs,network,nfs,nfslock,rpcbind,rpcgssd,rpcidmapd

