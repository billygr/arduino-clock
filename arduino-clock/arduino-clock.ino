// Arduino clock based on TM1637 and DS3231
#include <Wire.h>
#include <TM1637Display.h>
#include <RTClib.h>

// TM1637 Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3

// DS3231 is I2C based, module is connected to A4 (SDA), A5 (SCL)

// Set to false to display time in 12 hour format, or true to use 24 hour:
#define TIME_24_HOUR      false

// Create display and DS1307 objects.  These are global variables that
// can be accessed from both the setup and loop function below.
TM1637Display clockDisplay(CLK, DIO);

RTC_DS3231 rtc;

// Keep track of the hours, minutes, seconds displayed by the clock.
// Start off at 0:00:00 as a signal that the time should be read from
// the DS3231 to initialize it.
int hours = 0;
int minutes = 0;
int seconds = 0;

// Remember if the colon was drawn on the display so it can be blinked
// on and off every second.
bool blinkColon = false;


void setup() {
  // Setup function runs once at startup to initialize the display
  // and clock.

  // Setup Serial port to print debug output.
  Serial.begin(115200);
  Serial.println("Clock starting!");
  delay(3000); // wait for console opening

  // Setup the display.
  clockDisplay.setBrightness(1);
  // Setup the DS3231 real-time clock.
  rtc.begin();

  // Set the clock if it hasn't been set before.

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
  /*
  bool setClockTime = !rtc.isrunning();
  // Alternatively you can force the clock to be set again by
  // uncommenting this line:
  //setClockTime = true;
  if (setClockTime) {
    Serial.println("Setting DS3231 time!");
    // This line sets the DS1307 time to the exact date and time the
    // sketch was compiled:
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // Alternatively you can set the RTC with an explicit date & time, 
    // for example to set January 21, 2014 at 3am you would uncomment:
    //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
*/
  
}

void loop() {
  // Loop function runs over and over again to implement the clock logic.
  
  // Check if it's the top of the hour and get a new time reading
  // from the RTC clock.  This helps keep the clock accurate by fixing
  // any drift.
  if (minutes == 0) {
    // Get the time from the DS3231.
    DateTime now = rtc.now();
    // Print out the time for debug purposes:
    Serial.print("Read date & time from DS3231: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    // Now set the hours and minutes.
    hours = now.hour();
    minutes = now.minute();
  }

  // Show the time on the display by turning it into a numeric
  // value, like 3:30 turns into 330, by multiplying the hour by
  // 100 and then adding the minutes.
  int displayValue = hours*100 + minutes;

  // Do 24 hour to 12 hour format conversion when required.
  if (!TIME_24_HOUR) {
    // Handle when hours are past 12 by subtracting 12 hours (1200 value).
    if (hours > 12) {
      displayValue -= 1200;
    }
    // Handle hour 0 (midnight) being shown as 12.
    else if (hours == 0) {
      displayValue += 1200;
    }
  }

  // Now print the time value to the display.
  clockDisplay.showNumberDec(displayValue);

  // Blink the colon by flipping its value every loop iteration
  // (which happens every second).
  blinkColon = !blinkColon;
//  clockDisplay.drawColon(blinkColon);
if (blinkColon)    clockDisplay.showNumberDecEx(displayValue, (0x80 >> 1), true);
 else   clockDisplay.showNumberDecEx(displayValue, (0x80 >> 0), true);

  // Pause for a second for time to elapse.  This value is in milliseconds
  // so 1000 milliseconds = 1 second.
  delay(1000);

  // Now increase the seconds by one.
  seconds += 1;
  // If the seconds go above 59 then the minutes should increase and
  // the seconds should wrap back to 0.
  if (seconds > 59) {
    seconds = 0;
    minutes += 1;
    // Again if the minutes go above 59 then the hour should increase and
    // the minutes should wrap back to 0.
    if (minutes > 59) {
      minutes = 0;
      hours += 1;
      // Note that when the minutes are 0 (i.e. it's the top of a new hour)
      // then the start of the loop will read the actual time from the DS1307
      // again.  Just to be safe though we'll also increment the hour and wrap
      // back to 0 if it goes above 23 (i.e. past midnight).
      if (hours > 23) {
        hours = 0;
      }
    }
  }

  // Loop code is finished, it will jump back to the start of the loop
  // function again!
}
