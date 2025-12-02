#include <Multi_Timer_V2.h>

/* Demonstrate Multi_Timer_V2 off-delay timer function
and timer done one-shot function

Operation and expected result:
- Connect an LED to pin D10 with appropriate current limiting
resistor R1.

       LED    R1
 D10 --->|---/\/\/--- GND

- Start the IDE serial monitor. Insure baud rates between
processor and monitor match.

Connect D4 to GND with either a switch or breadboard jumper.

While the timer is running and has not reached preset the
LED on D10 will light.  Once preset is reached LED on D10 goes off.
and the built-in LED turns on.  When
switch is off or jumper is removed built-in LED turns off.

When the timer reaches preset a counter is incremented 
and the value is displayed on the IDE serial monitor.
*/

OffDelayTimer myTimer1(1000);

int counter1 = 0;
byte timerRunning = 10;
byte switch1 = 4;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(timerRunning, OUTPUT);
  pinMode(switch1, INPUT_PULLUP);
}

void loop() {
  myTimer1.update();  // refresh the timer value and flags

  // Enable/disable the timer using the ternary operator
  myTimer1.setEnable(digitalRead(switch1) ? false : true);

  // Turn an output on or off using the ternary operator
  digitalWrite(LED_BUILTIN, myTimer1.isDone() ? HIGH : LOW);

  // Turn an output on or off using if/else
  if (myTimer1.isRunning()) {
    digitalWrite(timerRunning, HIGH);
  } else digitalWrite(timerRunning, LOW);

  // Show the updated counter value
  if (myTimer1.getDoneFell()) {
    Serial.println(++counter1);
  }

  // // diagnostic
  // if (myTimer1.isEnabled() == true)
  //   counter1--;
  // Serial.println(counter1);
}