# ST7735 library for Raspberry PI

A port of [ST7735 Arduino library](https://github.com/adafruit/Adafruit-ST7735-Library) and [ST7735 GFX Arduino library](https://github.com/adafruit/Adafruit-GFX-Library) to Raspberry PI using [wiringPi](https://github.com/WiringPi). The idea was to be able to control ST7735 TFT LCD on Raspberry PI using C++ instead of the regular Python libraries.

Supported screens are :

* [Adafruit ST7735 1.44'](https://www.adafruit.com/product/2088)
* [Adafruit ST7735 1.8'](https://www.adafruit.com/product/358)

Features implemented :

- [x] fill screen
- [x] draw rectangle
- [x] draw text
- [x] draw bitmap
- [ ] read bitmap from SD card
- [ ] draw vertical line
- [ ] rotate screen
- [ ] custom fonts
- [ ] all other features of Adafruit-ST7735-Library


## Prerequesites

* wiringPi
* cmake

## Install library

On your Raspberry PI :

```bash
git clone git://github.com/bertrandmartel/st7735-raspberry
cd st7735-raspberry
cmake .
make
sudo make install
```

* Check the testing example (you may need to edit RST and DC pin inside `examples/main.cpp` :

```bash
./out/examples/main
```

## Wiring

|  description  |    WiringPi    |   Physical pin|
|---------------|----------------|------------|
|     VCC       |       -        |      2     |
|     GND       |       -        |     14     |
|     DC        |       5        |     18     |
|    MOSI       |       12       |     19    |
|    RST      |       6        |     22    |
|    CS         |       10       |     24    |
|    SCK        |       14       |    23    |

In the library only RST and DC can be configured :

Check the pin diagram with `gpio readall`

![st7735](https://user-images.githubusercontent.com/5183022/76146465-3f7f7c80-6093-11ea-9062-f3aa7439f023.png)

1.44'            |  1.8'
:-------------------------:|:-------------------------:
![st7735](https://user-images.githubusercontent.com/5183022/76146461-3db5b900-6093-11ea-872a-24592b153a74.jpg)  | ![st7735](https://user-images.githubusercontent.com/5183022/76146463-3ee6e600-6093-11ea-850f-7b5cd5ef2b61.jpg)


## Using the library

```cpp
#include "st7735.h"
#include "wiringPi.h"

#define TFT_RST  6
#define TFT_DC   5

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_DC, TFT_RST);

void setup() {
    wiringPiSetup();
    //tft.initR(INITR_144GREENTAB); //for 1,44'
    tft.initR(INITR_BLACKTAB); //for 1.8'
}

int main() {
    setup();
    //fill screen with color
    tft.fillScreen(ST7735_BLACK);

    //draw a rectangle
    //tft.fillRect(10, 10,  ST7735_TFTWIDTH_128 - 20, ST7735_TFTHEIGHT_128-20, ST7735_WHITE); //for 1.44'
    tft.fillRect(10, 10,  ST7735_TFTWIDTH_128 - 20, ST7735_TFTHEIGHT_160-20, ST7735_WHITE); //for 1.8'

    delay(750);

    tft.fillScreen(ST7735_BLACK);
    //draw text
    tft.setTextBackground(ST7735_BLACK);
    char hello[] = "Hello world !\nHello world !\n";
    tft.drawString(hello, ST7735_WHITE, 1);
    delay(1000);
    tft.drawString(hello, ST7735_GREEN, 1);
    delay(1000);

    tft.drawString("H", ST7735_BLUE, 1);
    tft.drawString("E", ST7735_BLUE, 1);
    tft.drawString("L", ST7735_WHITE, 1);
    tft.drawString("L", ST7735_WHITE, 1);
    tft.drawString("O", ST7735_RED, 1);
    tft.drawString("!", ST7735_RED, 1);

    //draw bitmap
    //tft.draw_bitmap_st7735(0, ST7735_TFTHEIGHT_128, imageLogo, ST7735_TFTWIDTH_128, ST7735_TFTHEIGHT_128); //for 1.44'
    //tft.draw_bitmap_st7735(0, ST7735_TFTHEIGHT_160, imageLogo, ST7735_TFTWIDTH_128, ST7735_TFTHEIGHT_160); //for 1.8'

    return 0;
}
```

## Compile code linking to library

```bash
g++ main.cpp -lwiringPi -lst7735 -o main
```

## Cross compilation

Get the [raspberry-dev](https://github.com/bertrandmartel/raspberry-dev) submodule by issuing : 

```bash
git submodule update --init --recursive
```

Use the `build.sh` script to cross compile the library :

```bash
./build.sh
```

This will invoke `cmake . -DCMAKE_TOOLCHAIN_FILE=./raspberry-dev/toolchain.cmake` which will automatically download the toolchain and wiringPI library 

## License

```
The MIT License (MIT)

Copyright for portions of project st7735-raspberry are held by Limor Fried/Ladyada for Adafruit Industries 
as part of projects Adafruit-ST7735-Library and Adafruit-GFX-Library 

All other copyright for project st7735-raspberry are held by Bertrand Martel, 2020.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```