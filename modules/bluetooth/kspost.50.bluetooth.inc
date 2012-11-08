# bluetooth service activates itself by default, we don't want that at
# the moment
systemctl disable bluetooth.service
