FQBN := arduino:mbed_nano:nano33ble 
PORT := /dev/serial/by-id/usb-Arduino*
CLI  := ./arduino-cli

install_impulse_lib:
	$(CLI) lib uninstall laura_skirt_v2_inferencing
	$(CLI) lib install --zip-path ei-laura-skirt-latest.zip

compile:
	$(CLI) compile --fqbn $(FQBN) --log --output-dir artifacts --export-binaries

upload:
	$(CLI) upload --fqbn $(FQBN) -p $(PORT)

monitor:
	$(CLI) monitor --fqbn $(FQBN) -p $(PORT)

install_daemon:
#    wget https://cdn.edgeimpulse.com/firmware/arduino-nano-33-ble-sense.zip
	cd ei-daemon-fw; PATH=../:$(PATH) ./flash_linux.sh
