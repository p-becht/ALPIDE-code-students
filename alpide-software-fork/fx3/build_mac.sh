g++ -dynamiclib libcyusb.c -o libcyusb.0.1.dylib -l usb-1.0
ln -sf libcyusb.0.1.dylib libcyusb.dylib

g++ -o download_fx3             download_fx3.c              -lcyusb -L. -l usb-1.0

