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

//TODO: make sure this is really milliseconds
//time elapsed (in miliseconds)
int timestamp = 0;

//timer interval (in miliseconds)
int time_interval = 500;

//the timer object
SimpleTimer timer;

//a timer ID
int timer_id;




//===========================================================
//variables for the SD card
//===========================================================

//SD card chip select pin
const int chip_select = 10;//for adafruit feather

//the opened file object
File active_file;

//keeps track of how many lines have been written to SD buffer
int flush_counter = 0;

//how often to force flushing data from the SD card buffer to the SD card
int flush_threshold = 1000;

//the file number (data_#.txt)
int file_number = 0;

//the name of the file
String file_name = "data_0.txt";

//a pointer to the contents of the file name string
const char *cstr_file_name = file_name.c_str();

//@Wyatt - remember the stuf I was talking about strings and making and deleting  pointers to char arrays? forget all of that. Modify the string file_name, and then use cstr_file_name for printing ie. for the display and for the SD card. It magically knows the "contents" of the string.

//a line of data formatted ("engine_rpm_counter,gearbox_rpm_counter,timestamp") for writing to the SD
String data_line;

//a pointer to the contents of the data string
const char *cstr_data_line = data_line.c_str();




//===========================================================
//variables for the display
//===========================================================

//display object
U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(U8X8_PIN_NONE);





//===========================================================
//toggles and control variables
//===========================================================

//NOTE: some of these might need to be volatile? because they will be the things changing inside of interrupts

//if true, the start/stop logging button was just pressed
volatile bool logging_toggle = false;

//if true, actively logging
bool logging = false;

//if true, the timer callback just ran -> time to write data
volatile bool timer_trigger = false;






//===========================================================
//pins and counters
//===========================================================

//the engine rpm pin
const int engine_rpm_pin = 1; //TODO: set this to wherever the Mantas special is hooked up

//how many times has the engine sparked in the past timer cycle
volatile int engine_rpm_counter = 0;

//the gearbox rpm pin
const int gearbox_rpm_pin = 2; //TODO: set this to wherever the gearbox hall effect sensor is plugged in

//how many times has the engine sparked in the past timer cycle
volatile int gearbox_rpm_counter = 0;

//the button to start/stop data collection
const int logging_pin = 3; //TODO: look up which pins the A/B/C buttons on the adafruit display are connected to




//===========================================================
//random stuff that has no better place
//===========================================================

//@Wyatt - remember the stuf I was talking about strings and making and deleting  pointers to char arrays? forget all of that.
//const char *cstr_file_name = file_name.c_str();

// callback vs interrupt
// pointers * vs references &

//https://stackoverflow.com/questions/7352099/stdstring-to-char/7352131
//https://stackoverflow.com/questions/347949/how-to-convert-a-stdstring-to-const-char-or-char
//https://www.geeksforgeeks.org/pointers-vs-references-cpp/





//===========================================================
//function declarations to keep the compiler happy
//also interrupt functions
//===========================================================

// a function that searches the SD card for which files exist, increments file number, creates the new file, and opens it for writing
void make_file();

// a hardware interrupt that runs whenever the engine rpm pin is high, increments the engine rpm counter
void engine_interrupt(){
    engine_rpm_counter++;
}

// a hardware interrupt that runs whenever the gearbox rpm pin is high, increments the gearbox rpm counter
void gearbox_interrupt(){
    gearbox_rpm_counter++;
}

// a hardware interrupt that runs whenever the logging button is pressed -> sets logging_toggle to true
void logging_interrupt(){
    logging_toggle = true;
}

// a callback function (not an interrupt... I think) that is called whenever the timer reaches a set interval -> sets the timer trigger to true to keep minimal stuff happening inside (just in case it is an interrupt)
void timer_cb(){
    timer_trigger = true;
}


//===========================================================
//the main code
//===========================================================


void setup() {
    //start the display, make sure the SD card exists, and initialize pins

    //start the display
    u8x8.begin();
    u8x8.setPowerSave(0);
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.clearDisplay();


    //start the SD
    if (!SD.begin(chip_select)) {
        u8x8.draw2x2String(0,0,"err: No SD");
        while(1);//loop forever
    }

    //TODO: make sure these pins are appropriately set to INPUT or INPUT_PULLUP
    //start the pins

    //attach logging interrupt
    attachInterrupt(digitalPinToInterrupt(logging_pin), logging_interrupt, RISING);


    u8x8.clearDisplay();
    //u8x8.draw1x2String(0,0,cstr_file_name);
    u8x8.draw2x2String(0,0,"CWRU");
    u8x8.draw2x2String(0,2,"Motorsports");
}

void loop() {
    //start logging mode (if the logging button has been pressed but we are not currently logging)
    if (logging_toggle && !logging){
        //start logging, end toggle
        logging_toggle = false;
        logging = true;

        //iterate file names and open new file
        make_file();

        //display that we're logging
        u8x8.clearDisplay();
        u8x8.draw2x2String(0,0,"Logging:");
        u8x8.draw2x2String(0,0,cstr_file_name);

        //clear counters
        engine_rpm_counter = 0;
        gearbox_rpm_counter = 0;

        //reset timestamp
        timestamp = 0;

        //attach interrupts
        attachInterrupt(digitalPinToInterrupt(engine_rpm_pin), engine_interrupt, RISING);
        attachInterrupt(digitalPinToInterrupt(gearbox_rpm_pin), gearbox_interrupt, RISING);
        //attachInterrupt(digitalPinToInterrupt(pin), ISR, mode)
        //mode can be LOW/CHANGE/RISING/FALLING/HIGH
        //ISR is the interrupt service routine (function called by interrupt)
        
        //TODO: start timer?
        timer_id = timer.setInterval(time_interval, timer_cb);
    }

    //logging mode
    if (logging){
        timer.run(); //check how much time has elapsed, potentially call the timer callback setting timer_trigger to true
        if (timer_trigger){
            //write data do associated work
            //increment time
            timestamp = timestamp + time_interval;

            //format data for writing
            data_line = String(engine_rpm_counter) + "," + String(gearbox_rpm_counter) + "," + String(timestamp);

            //write data to SD buffer
            active_file.println(cstr_data_line);
            //NOTE: this is writing to the SD BUFFER and not the SD itself, need to flush periodically to make sure the data is actually written. Otherwise, if the SD shakes loose (it's happened) the data in the buffer will get lost
            //TODO: maybe println returns if it was successful -> add an error message if it stops writing?

            //TODO: implement flush forcing loop

            //clear counters
            engine_rpm_counter = 0;
            gearbox_rpm_counter = 0;

            //reset timer trigger
            timer_trigger = false;
        }
        //otherwise do nothing -> the hardware interrupts are doing the counting work
    }

    //stop logging mode (if the logging button has been pressed and we are currently logging)
    if (logging_toggle && logging){
        
        //force a final data flush and close the file
        active_file.flush();
        active_file.close();

        //detatch interrupts (deactivate them)
        detachInterrupt(digitalPinToInterrupt(engine_rpm_pin));
        detachInterrupt(digitalPinToInterrupt(gearbox_rpm_pin));

        //TODO: stop timer?
        //https://playground.arduino.cc/Code/SimpleTimer/

        //display that we stopped logging
        u8x8.clearDisplay();
        u8x8.draw2x2String(0,0,"FAPPY NOB");
        u8x8.draw2x2String(0,2,"GET WRECKD");
    }
    //otherwise, do nothing
}

void make_file(){
    
    while (SD.exists(cstr_file_name)){
        file_number++;
        file_name = "data_" + String(file_number) + ".txt";
        //char *cstr_file_name = file_name.c_str();
        u8x8.draw1x2String(0,0,cstr_file_name);
    }

    active_file = SD.open(cstr_file_name, FILE_WRITE);
    // active_file.println("test");
    // active_file.close();
}


