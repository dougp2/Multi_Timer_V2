#include <Multi_Timer_V2.h>

/* Demonstrate Multi_Timer_V2 retentive timer function

Operation and expected result:

Connect input D4 to GND with either a SPST switch or breadboard
jumper.  Do the same for D2.

- Start the IDE serial monitor. Insure baud rates between
processor and monitor match.

When D4 (switch1) is closed the timer advances, providing reset
is not active.  When switch1 is open timing stops and accumulated
value is retained.  When switch1 is closed again timing picks up
from the saved value.  isDone() goes true when preset is reached.
The timer will stay in its last state until reset goes true.
When reset goes true (resetSwitch is pressed) isDone() goes false
and accumulated value is cleared to zero.
*/

const unsigned long timerInterval = 1300;

// Instantiate a Multi_Timer_V2 object
RetentiveTimer myTimer1(timerInterval);

int counter1 = 0;

byte externalLED = 10;
byte switch1 = 4;
byte resetSwitch = 2;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(switch1, INPUT_PULLUP);
  pinMode(resetSwitch, INPUT_PULLUP);
}

void loop() {
  myTimer1.update();  // refresh the timer value and flags

  // Enable/disable the timer using the ternary operator
  myTimer1.setCtrl(digitalRead(switch1) == HIGH ? false : true);

  // This is the timer output
  digitalWrite(LED_BUILTIN, myTimer1.isDone() ? HIGH : LOW);

  myTimer1.setReset(digitalRead(resetSwitch) == HIGH ? false : true);

  Serial.println(myTimer1.getCount());
}
