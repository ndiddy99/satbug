Packages to install: libftdi-dev, libusb-1.0-0-dev

If you get "Inappropriate permissions on device" error, put make a file with this in it at /etc/udev/rules.d/50-ftdi.rules:

SUBSYSTEMS=="usb", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", MODE="0666"

and make sure to replug the devcart usb cable after making the file
