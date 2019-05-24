//The new and improved DAQ platform

#include <Arduino.h>//use the arduino framework
#include <SPI.h>//library for using the SPI interface (needed for SD)
#include <SD.h>//library for SD file management - use the one made by Arduino
#include <SimpleTimer.h>//a simple timer interrupt object
#include <U8x8lib.h>//display driver

//uncomment if running with serial output "debugging"
//#define SERIAL_DEBUGGING 1

//===========================================================
//variables for the timer
//===========================================================

//time elapsed (in miliseconds)
int time_elapsed = 0;
//timer interval (in miliseconds)
int time_interval = 500;
//the timer object
SimpleTimer timer;



//===========================================================
//variables for the SD card
//===========================================================

//SD card chip select pin
const int chip_select = 10;//for adafruit feather
//the file number (data_#.txt)
int file_number = 0;
//the name of the file
String file_name = "data_0.txt";
//a pointer to the contents of the string
const char *cstr_file_name = file_name.c_str();
//file_name.c_str();
//the opened file object
File active_file;



//===========================================================
//variables for the display
//===========================================================

//display object
U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(U8X8_PIN_NONE);



//===========================================================
//random stuff that has no better place
//===========================================================

//modify string, create pointer
//const char *cstr_file_name = file_name.c_str();

// callback vs interrupt
// pointers * vs references &

//https://stackoverflow.com/questions/7352099/stdstring-to-char/7352131
//https://stackoverflow.com/questions/347949/how-to-convert-a-stdstring-to-const-char-or-char
//https://www.geeksforgeeks.org/pointers-vs-references-cpp/



void setup() {
    //start the display
    u8x8.begin();
    u8x8.setPowerSave(0);
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.clearDisplay();
    u8x8.println("booting...");
    //u8x8.draw2x2String(0,0,packetBuffer);


    //SD.begin(chip_select);
    if (!SD.begin(chip_select)) {
        u8x8.println("no SD!");
        while(1);//loop forever
    }


    u8x8.clearDisplay();
    u8x8.draw1x2String(0,0,cstr_file_name);

    while (SD.exists(cstr_file_name)){
        file_number++;
        file_name = "data_" + String(file_number) + ".txt";
        //char *cstr_file_name = file_name.c_str();
        u8x8.draw1x2String(0,0,cstr_file_name);
    }

    active_file = SD.open(cstr_file_name, FILE_WRITE);
    active_file.println("test");
    active_file.close();
}

void loop() {
    // put your main code here, to run repeatedly:
}


