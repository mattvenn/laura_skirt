# laura skirt

use nano33ble + edge impulse to flash lights on Laura's skirt depending on what movement she makes.

demo: https://photos.app.goo.gl/jk241ZbBRNBDa5vi6

* qs about improvement https://forum.edgeimpulse.com/t/non-continuous-gesture-recognition/9276

## Setup with Arduino cli

Need [arduino-cli](https://arduino.github.io/arduino-cli/0.35/installation/) installed to ./firmware. Then:



    cd firmware

Install config

    arduino-cli config init 

then edit the config (/home/matt/.arduino15/arduino-cli.yaml) and enable_unsafe_install to be able to install local zips
and install libs

    arduino-cli lib install --git-url https://github.com/adafruit/Adafruit_NeoPixel
    arduino-cli lib install Arduino_LSM9DS1

download latest edge impulse to ei-laura-skirt-latest.zip
and update local library:

    make install_impulse_lib

compile

    make compile

upload

    make upload

can fails to find serial port. but double tap reset to force it.

## edge-impulse-daemon

To collect data.

* install npm, then data forwarder: https://docs.edgeimpulse.com/docs/tools/edge-impulse-cli/cli-installation
* need firmware on the board: https://docs.edgeimpulse.com/docs/development-platforms/officially-supported-mcu-targets/arduino-nano-33-ble-sense
* need arduino-cli: https://arduino.github.io/arduino-cli/0.35/installation/

This results in arduino-cli installed in a local bin directory, so to flash:

    make install_daemon

To connect to edge impulse:

    edge-impulse-daemon

Board needs to show here: https://docs.edgeimpulse.com/docs/tutorials/end-to-end-tutorials/continuous-motion-recognition#1.-prerequisites
For my project the connected devices are here: https://studio.edgeimpulse.com/studio/19877/devices

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
* gave up on platformio after not being able to solve this: https://forum.edgeimpulse.com/t/can-no-longer-build-nano33ble-due-to-mbed-compilation-issue/9266/7
