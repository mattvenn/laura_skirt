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
