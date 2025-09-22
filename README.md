An Arduino library containing several types of non-blocking millis timers for general use.&nbsp; 
Any reasonable number of timers in any combination can be used.&nbsp;  The timers must be
updated repeatedly in loop() and two calls are provided for this - one for updating a single
timer instance and another for updating all timers at once.

The example sketches were tested on a standard Aduino NANO and developed on an UNO R3.

**Updating timers:**
To update a single timer use \<your timer name\>.update() and to update all timers use 
 Multi_Timer::updateAllTimers() .

  **Controlling the timers :**

 myTimer1.setEnable(bool) - Set to true enables a timer to run, provided the timer is
 not being held reset by .setReset(bool).  Set to false the timer resets or stops.

 myTimer1.setReset(bool) - Set to false a timer is enabled to run.&nbsp; Set to true a timer is
 reset to a zero accumulated value and .isDone() will return false.&nbsp; The timer will be
 held at reset until setReset is set false.

 myTimer1.setCtrl(bool) - A few types of timers use this as a special start signal.
 
# Getting information from the timers :

**bool myTimer1.isDone()** - Returns true if the timer has reached its preset value.&nbsp; Returns
false otherwise.

**bool myTimer1.isRunning()** - Returns a boolean true if the timer is actively timing and has not
reached its preset value.&nbsp; Returns false otherwise.

**myTimer1.isEnabled()** - Returns a boolean indicating whether or not the timer is enabled to run.
 
**myTimer1.isReset()** - Returns a boolean indicating whether or not a timer is being held reset.

**myTimer1.getDoneRose()** - Returns a boolean which is true for one program scan when the timer
reaches its preset value.

**bool myTimer1.getDoneFell()** - Returns a boolean which is true for one program scan when the timer is reset, if
the timer has reached its preset value.

**unsigned long myTimer1.getCount()** - Returns the current timer accumulated value.


  **Timer types**

_OnDelayTimer myTimer1(UL)_ - A timer which runs when reset is false and enable is true.&nbsp; 
If reset is true or enable is false it is reset.&nbsp; isDone(bool) goes true when preset is reached,
false otherwise.

_OffDelayTimer(UL)_ - Off delay timer functions like an on delay timer but enable and done
operate inversely to an on delay timer.&nbsp; Timer is reset to zero accumulated value and isDone() is true
when isEnabled() is true.&nbsp; Timing cycle begins when enable goes false and after the delay time isDone()
goes false.

_RetentiveTimer(UL)_ - A timer which accumulates when enabled and reset is false.&nbsp;   Accumulated value
is retained when enable is false.&nbsp; This timer type is reset only by making the reset input true.

_PulseGenTimer(UL)_ - A timer which runs when enabled and not reset.&nbsp; Resets itself upon reaching preset
then restarts the timing cycle automatically as long as enable is true.

_RetriggerableTimer(UL)_ - When the timer is enabled a change of state on the 'control' input resets the timer
and restarts the timing cycle.&nbsp; Continuous cycling of the control input at a rate faster than the time
delay will cause the isDone() status flag to remain low indefinitely.

_FlasherTimer(UL, UL)_ - This timer runs and resets itself automatically when enabled.&nbsp; It is basically
an enhanced pulse generator timer.&nbsp; The first constructor argument specifies the period of the timer, say
one second.&nbsp; The second constructor argument specifies an ON time for a special output <.isFlashing(bool)> unique
to this type.&nbsp; The ON time begins when timing starts and ends when ON time milliseconds is reached.
If Enable goes false the timer is reset immediately.

Set onTime to some fraction of the preset value.&nbsp; The onTime value can be runtime modified using the function
myTimer1.setOnTime(UL).&nbsp; If onTime is modified at runtime the timer is immediately reset.

Flasher timer can be used as a pulse generator timer but the isFlashing() and setOnTime() features will
incur a penalty in memory and clock cycle usage over a PulseGenTimer.


