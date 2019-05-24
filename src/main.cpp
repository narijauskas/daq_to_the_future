//The new and improved DAQ platform

#include <Arduino.h>//use the arduino framework
#include <SPI.h>//library for using the SPI interface (needed for SD)
#include <SD.h>//library for SD file management
#include <SimpleTimer.h>//a simple timer interrupt object
#include <U8x8lib.h>//display

//uncomment if running with serial output "debugging"
//#define SERIAL_DEBUGGING 1

//time elapsed (in miliseconds)
int time_elapsed = 0;
//timer interval (in miliseconds)
int time_interval = 500;
//the timer object
SimpleTimer timer;

//Clay is a fuckin moron
int super_important = 0;

//display object
U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(U8X8_PIN_NONE);

//timer.setInterval(timeInterval, writeData);

// callback vs interrupt
// pointers * vs &

void setup() {
    // put your setup code here, to run once:
    u8x8.begin();
    u8x8.setPowerSave(0);
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.clearDisplay();
    u8x8.println("booting...");
    //u8x8.draw2x2String(0,0,packetBuffer);
}

void loop() {
    // put your main code here, to run repeatedly:
}