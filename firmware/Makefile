FQBN := arduino:mbed_nano:nano33ble 
PORT  := /dev/serial/by-id/usb-Arduino*
CLI  := ../ei-daemon-fw/bin/arduino-cli

install:
	$(CLI) lib install --zip-path ei-laura-skirt-arduino-1.0.11.zip

compile:
	$(CLI) compile --fqbn $(FQBN) --log --output-dir artifacts --export-binaries

upload:
	$(CLI) upload --fqbn $(FQBN) -p $(PORT)

monitor:
	$(CLI) monitor --fqbn $(FQBN) -p $(PORT)
