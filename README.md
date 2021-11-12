# esp32-ds3231-dht11-p5-rgb-32x64
esp32 ds3231 dht11 p5 matrix rgb 32x64

materials:

esp32, p5 rgb 32x64 (1 pcs), rtc ds3231, dht11, psu 5v/10A, breadboard, jumper cable, data cable hub75

# Attention!
Change this code in the file RGBmatrixPanel.cpp in your RGB Matrix_panel Library: 

#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_ESP32)
  // R1, G1, B1, R2, G2, B2 pins
  
  // static const uint8_t defaultrgbpins[] = {2, 3, 4, 5, 6, 7};  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;// ---> for Arduino board
  
  static const uint8_t defaultrgbpins[] = {25, 26, 27, 12, 13, 14}; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;// ---> for ESP32 board
  
  memcpy(rgbpins, pinlist ? pinlist : defaultrgbpins, sizeof rgbpins);
  

![alt text](https://github.com/jenizar/esp32-ds3231-dht11-p5-rgb-32x64/blob/main/screenshot/ss1.jpg)
![alt text](https://github.com/jenizar/esp32-ds3231-dht11-p5-rgb-32x64/blob/main/screenshot/ss2.jpg)
![alt text](https://github.com/jenizar/esp32-ds3231-dht11-p5-rgb-32x64/blob/main/screenshot/ss3.jpg)
![alt text](https://github.com/jenizar/esp32-ds3231-dht11-p5-rgb-32x64/blob/main/screenshot/ss4.jpg)

references:
1. https://youtu.be/0gGnr2HYCnQ
2. https://arduino-projects-free.blogspot.com/2021/06/esp32-ds1307-dht11-matrix-p4-rgb-64x32.html
