# Disable package gpg check until F-14 arm repos are signed
sed -i.rpmsave 's/gpgcheck=1/gpgcheck=0/' /etc/yum.repos.d/fedora.repo
sed -i.rpmsave 's/gpgcheck=1/gpgcheck=0/' /etc/yum.repos.d/fedora-updates.repo
