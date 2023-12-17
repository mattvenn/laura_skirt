# laura skirt

use nano33ble + edge impulse to flash lights on Laura's skirt depending on what movement she makes.

## compile & flash

go to https://studio.edgeimpulse.com/studio/19877/deployment

* in the search box in the middle find 'arduino library'
* at the bottom of the page click 'build'
* then download and move to local dir
* update the simlink ei-laura-skirt.zip to point to the new download

to compile:

    pio build

to flash:

    pio run --target upload

currently fails with this: https://forum.edgeimpulse.com/t/can-no-longer-build-nano33ble-due-to-mbed-compilation-issue/9266
also fails on github action

## Arduino cli attempts

Install config

    ../ei-daemon-fw/bin/arduino-cli config init 

then edit the config (/home/matt/.arduino15/arduino-cli.yaml) and enable_unsafe_install to be able to install local zips
and install libs

    ../ei-daemon-fw/bin/arduino-cli lib install --zip-path ./ei-tester-arduino-1.0.1.zip
    ../ei-daemon-fw/bin/arduino-cli lib  install --git-url https://github.com/adafruit/Adafruit_NeoPixel
    ../ei-daemon-fw/bin/arduino-cli  lib install Arduino_LSM9DS1

compile

    ../ei-daemon-fw/bin/arduino-cli  compile --fqbn arduino:mbed_nano:nano33ble

prints no logging! makes the fans whir. after a while finishes!

upload

    ../ei-daemon-fw/bin/arduino-cli  upload --fqbn arduino:mbed_nano:nano33ble -p /dev/serial/by-id/usb-Arduino_Nano_33_BLE_3C48BB3E0BD44A03-if00

fails to find serial port. but double tap reset, get different serial port and this works:

    ../ei-daemon-fw/bin/arduino-cli  upload --fqbn arduino:mbed_nano:nano33ble -p /dev/serial/by-id/usb-Arduino_Arduino_Nano_33_BLE_00000000000000003C48BB3E0BD44A03-if00

## edge-impulse-daemon

To collect data.

* install npm, then data forwarder: https://docs.edgeimpulse.com/docs/tools/edge-impulse-cli/cli-installation
* need firmware on the board: https://docs.edgeimpulse.com/docs/development-platforms/officially-supported-mcu-targets/arduino-nano-33-ble-sense
* need arduino-cli: https://arduino.github.io/arduino-cli/0.35/installation/

This results in arduino-cli installed in a local bin directory, so to flash:

    PATH=./bin:$PATH ./flash_linux.sh 

To connect to edge impulse:

    edge-impulse-daemon

Board needs to show here: https://docs.edgeimpulse.com/docs/tutorials/end-to-end-tutorials/continuous-motion-recognition#1.-prerequisites
For my project the connected devices are here: https://studio.edgeimpulse.com/studio/19877/devices

## collecting new data with data forwarder

can't seem to turn off the unwanted sensor data. trying the dataforwarder with this firmware: https://docs.edgeimpulse.com/docs/tools/edge-impulse-cli/cli-data-forwarder#example-arduino

put the build into data_forwarder_fw and setup new pio project.

    pio run -t upload --upload-port /dev/serial/by-id/usb-Arduino_Nano_33_BLE_3C48BB3E0BD44A03-if00

start forwarder:

    edge-impulse-data-forwarder --frequency 62.5

the names before were accX,accY,accZ - were they automatically provided from somewhere?

## issues

### nano ble 

* https://docs.arduino.cc/hardware/nano-33-ble-sense
* https://docs.arduino.cc/resources/datasheets/ABX00031-datasheet.pdf

* If can't upload, quickly press the button twice.
* Use this port /dev/serial/by-id/usb-Arduino_Nano_33_BLE_3C48BB3E0BD44A03-if00
* If that fails with pio, then waiting for it to start trying then double press reset works
* After updating fw, the nano disappeared from serial ports, forced the nrf platform to nordicnrf52@9.5.0 after finding [this](https://community.platformio.org/t/nano33ble-device-serial-port-hangs-after-upload-attempt-using-clion-platformio-on-m2-mac/33508/3)
* FastLED still not supported :(

### pio

* monitor: pio device monitor
* build: pio run
* upload: pio run -t upload --upload-port /dev/serial/by-id/usb-Arduino_Nano_33_BLE_3C48BB3E0BD44A03-if00
