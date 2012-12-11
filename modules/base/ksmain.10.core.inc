# LANG=C considerably speeds up boot (measured 5sec) -- bernie
lang C

keyboard us
auth --useshadow --enablemd5
selinux --disabled
firewall --disabled
xconfig --startxonboot
part / --size 4096 --fstype=ext4
services --enabled=crond,olpc-dm,olpc-configure,olpc-boot-finish,plymouth-shutdown-wait,powerd,olpc-kbdshim,olpc-switchd,runin-check --disabled=dnsmasq,ip6tables,iptables,mdmonitor,netfs,network,sshd
