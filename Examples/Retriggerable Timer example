#include <Multi_Timer_V2.h>

/* Demonstrate Multi_Timer_V2 retriggerable timer function

Operation and expected result:
- Connect an LED to pin D10 with appropriate current limiting
resistor R1.

       LED     R1
 D10 --->|---/\/\/--- GND

Connect input D4 to GND with either a SPST switch or breadboard
jumper.

- Start the IDE serial monitor. Insure baud rates between
processor and monitor match.

setEnable(true) is executed in setup() to allow the timer to do its
function.  setEnable(false) will disable the timer and output will
stay false.  Effectively the timer is held reset.

This timer uses a second special method/function setCtrl(bool) for
control rather than the usual setEnable(bool).

When D4 (switch1) is held either open or closed for longer than the
preset time the LED will turn on when preset time elapses.

If switch1 is repeatedly pressed and released at a rate faster than
the timer preset value the external LED will remain off. 

*/

const unsigned long timerInterval = 1300;

// Instantiate a Multi_Timer_V2 object
RetriggerableTimer myTimer1(timerInterval);

int counter1 = 0;

byte externalLED = 10;
byte switch1 = 4;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(externalLED, OUTPUT);
  pinMode(switch1, INPUT_PULLUP);
  myTimer1.setEnable(true);
 }

void loop() {
  myTimer1.update();  // refresh the timer value and flags

  // Enable/disable the timer using the ternary operator
  myTimer1.setCtrl(digitalRead(switch1) == HIGH ? false : true);

   
  // This is the timer output
  digitalWrite(externalLED, myTimer1.isDone());
}
