//The new and improved DAQ platform

#include <Arduino.h>//use the arduino framework
#include <SPI.h>//library for using the SPI interface (needed for SD)
#include <SD.h>//library for SD file management
#include <SimpleTimer.h>//a simple timer interrupt object
#include <U8x8lib.h>//display

//uncomment if running with serial output "debugging"
//#define SERIAL_DEBUGGING 1


//variables for the timer

//time elapsed (in miliseconds)
int time_elapsed = 0;
//timer interval (in miliseconds)
int time_interval = 500;
//the timer object
SimpleTimer timer;


//variables for the SD card

//SD card chip select pin
const int chip_select = 10;//for adafruit feather
String directory_name = "yop.txt";
String file_name = "yop";

//std::string str = "string";

const char *cstr = directory_name.c_str();
//https://stackoverflow.com/questions/7352099/stdstring-to-char/7352131
//https://stackoverflow.com/questions/347949/how-to-convert-a-stdstring-to-const-char-or-char

File active_file;


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


    //SD.begin(chip_select);
    if (!SD.begin(chip_select)) {
        u8x8.println("initialization failed!");
    }


    active_file = SD.open(cstr, FILE_WRITE);
    active_file.println("test");
    active_file.close();
}

void loop() {
    // put your main code here, to run repeatedly:
}