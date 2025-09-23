// filename: Multi_Timer_V2.cpp  6/18/24
//
// https://forum.arduino.cc/t/renamed-iterating-through-a-series-of-objects-original-title-oop-this-pointer-questions/1244434


#include "Multi_Timer_V2.h"
#include "Arduino.h"
//
/*
      Multi_Timer is the base timer class.
      > Sets done status true upon reaching preset (false for off timer).
      > Produces a positive-going one-scan pulse 'os' upon reaching
      preset value (done status = true) and another one-scan pulse
      when done goes false.
      > responds to a reset command by setting accumulated value to
      zero and resetting _Done and _TimerRunning.
    */
//----------------------------------------------------
//

Multi_Timer *Multi_Timer::first = nullptr;  // Initialize pointer variable 'first'

// One-argument constructor for non-FlasherTimer types - delegates
// to the two-argument constructor, removing need for object
// pointer setup loop.

Multi_Timer::Multi_Timer(unsigned long pre)
  : Multi_Timer(pre, 0UL) {  // trailing zero value is default onTime initialization
}

/*==============================================

    Two-argument constructor for FlasherTimer type.
---------------------------------------------------
Acts as delegate for one-argument constructor
*/
Multi_Timer::Multi_Timer(unsigned long pre, unsigned long onTime) {
  _Preset = pre;
  _OnTime = onTime;
  _Control = false;

  /*
  Set up pointers to list of objects to enable one call
  to Update_All_Timers() rather than each timer making 
  its own call to update().
  */

// https://forum.arduino.cc/t/renamed-iterating-through-a-series-of-objects-original-title-oop-this-pointer-questions/1244434/110
  next = nullptr;  // lets timers survive power cycle intact
  if (first == nullptr) {
    first = this;
  } else {

    //
    // Start at the first pointer and walk the list until we
    // find one that doesn't have a next
    Multi_Timer *ptr = first;
    while (ptr->next != nullptr) {
      ptr = ptr->next;
    }
    ptr->next = this;
  }
}

/*==========================

   Destructor - Comments from forum post #38
   https://forum.arduino.cc/t/renamed-iterating-through-a-series-of-objects-original-title-oop-this-pointer-questions/1244434/38?
 ------------------------*/
Multi_Timer::~Multi_Timer() {
  // If this is the first instance then we need to make the next one first
  // If there's no next instance then first becomes nullptr and the next
  // new instance will be first.
  if (first == this) {
    first = this->next;
  } else {
    // Start at the first instance and walk the list until we find the one
    // whose next pointer points to the one being destroyed.
    Multi_Timer *ptr = first;
    while (ptr->next != this) {
      ptr = ptr->next;
    }
    // Give that instance this one's next.  If there is no next
    // (ie this is last in the list) then ptr->next becomes nullptr
    // and marks the new end of the list.
    ptr->next = this->next;
  }
}
/*======================================

           Accessor methods
--------------------------------------*/

void Multi_Timer::setEnable(bool en) {
  _Enable = en;
}

bool Multi_Timer::isEnabled() const {
  return _Enable;
}

void Multi_Timer::setReset(bool rst){
  _Reset = rst;
}

bool Multi_Timer::isReset() const {
  return _Reset;
}

bool Multi_Timer::isDone() const {
  return _Done;
}

bool Multi_Timer::isRunning() const {
  return _TimerRunning;
}

void Multi_Timer::setCtrl(bool ctrl) {
  _Control = ctrl;
}

unsigned long Multi_Timer::getCount() const {
  return _Accumulator;
}

bool Multi_Timer::getDoneRose() const {
  return _Done_OSR;
}

bool Multi_Timer::getDoneFell() const {
  return _Done_OSF;
}

//========================================================

//  The 'update' function is the heart of the thing.
//  ----------------------------------------------------
//  Updates timer accumulated value & conditions flags '_Done',
// '_Done_Rising_OS', '_Done_Falling_OS' and '_TimerRunning'.
//
//  Returns boolean status of _Done.  update() must be called
//  periodically in loop to update the flags and accumulated
//  value.
//  ====================================================

bool Multi_Timer::update() {
  _CurrentMillis = millis();  // Get system clock ticks
  if (_Enable or _Control) {  // timer is enabled to run
    _Accumulator = _Accumulator + _CurrentMillis - _LastMillis;
    if (_Accumulator >= _Preset) {  // timer done?
      _Accumulator = _Preset;       // Don't let accumulator run away
      _Done = true;
    }
  }
  _LastMillis = _CurrentMillis;

  if (reset()) {  // Call virtual reset function.  Reset timer if
    //              returns true, based on derived class' criteria.
    _Done = false;
    _Accumulator = 0;
    _Control = false;  // ensures reset of latched type
  }
  /*-----
    Generate a one-scan momentary flag on _Done
    false-to-true transition
  */
  _Done_OSR = (_Done and _Done_Rising_Setup);  // timer done OS
  _Done_Rising_Setup = !_Done;

  /*----
    and another one-scan momentary flag on _Done
    true-to-false transition
  */
  _Done_OSF = (!_Done and _Done_Falling_Setup);  // timer not done OS
  _Done_Falling_Setup = _Done;

  /*
     Condition the timer running flag.
  */
  if ((_Enable or _Control) and !_Done and !_Reset) {
    _TimerRunning = true;
  } else _TimerRunning = false;

  return _Done;  // exit to caller
}  // end update function

/* ===================================================

              Update All Timers
   ---------------------------------------------------
   Loop through all timer objects and refresh them by
   successively calling update().
   ---------------------------------------------------
*/
void Multi_Timer::updateAllTimers() {
  for (Multi_Timer *ptr = first; ptr != nullptr; ptr = ptr->next) {
    ptr->update();
  }
}  // end of updateAllTimers

/* ================================================

            On Delay Timer class definition
 --------------------------------------------------------------------
 A timer which runs when reset is false and enable is true.
 It is reset otherwise. _Done goes true when preset is reached, 
 false otherwise.

 OnDelayTimer timer waveforms

enable      _________
       ____|         |___

                 ____
done  __________|    |___
           ^ Td ^ 

 --------------------------------------------------------------------
*/
OnDelayTimer::OnDelayTimer(unsigned long pre)
  : Multi_Timer(pre){};

OnDelayTimer::~OnDelayTimer() {
  Serial.println("one on delay timer destroyed");
}  // give a destructor

// Establish reset conditions for ON delay timer
bool OnDelayTimer::reset() {
  return (_Reset or !_Enable);
}  // End of OnDelay timer

/*=======================================================

               Off Delay Timer class definition
--------------------------------------------------------------------
A timer which runs when reset is false and _Enable is false.
If reset is true or enable is true the timer is held reset.

_Done goes false preset milliseconds after _Enable goes false.
This timer object redefines the base class update function.

OffDelayTimer waveforms

enable  ____          ___  
            |________|
            
done    _________     ___
                 |___|
            ^ Td ^ 

-------------------------------------------------------------------*/

OffDelayTimer::OffDelayTimer(unsigned long pre)
  : Multi_Timer(pre){};

OffDelayTimer::~OffDelayTimer() {
  }  // give a destructor

// update function/method is redefined here

bool OffDelayTimer::update() {
  bool enableOff = !_Enable;  // reverse sense of enable for TOF // original line
  _CurrentMillis = millis();  // Get system clock ticks
                              // Serial.println(enableOff);
  if (enableOff) {            // timer is enabled to run

    _Accumulator = _Accumulator + _CurrentMillis - _LastMillis;
    if (_Accumulator >= _Preset) {  // timer done?
      _Accumulator = _Preset;       // Don't let accumulator run away
      _Done = false;
    }
  }
  _LastMillis = _CurrentMillis;

  if (_Reset or !enableOff) {  // Find out if reset needed based on derived class' criteria.
    _Done = true;
    _Accumulator = 0;
  }
  /*-----
    Generate a one-scan momentary flag on _Done
    false-to-true transition
  */
  _Done_OSR = (_Done and _Done_Rising_Setup);  // timer done OS
  _Done_Rising_Setup = !_Done;

  /*----
    and another one-scan momentary flag on _Done
    true-to-false transition
  */
  _Done_OSF = (!_Done and _Done_Falling_Setup);  // timer not done OS
  _Done_Falling_Setup = _Done;

  /*
    ----- condition the timer running status
  */
  if ((enableOff) and _Done and !_Reset) {
    _TimerRunning = true;
  } else _TimerRunning = false;

  return !_Done;
}

bool OffDelayTimer::reset() {
  return (_Reset or _Enable);
}  // End of OffDelay timer

/*==============================================================

             Retentive timer class definition
--------------------------------------------------------------
A timer which accumulates when enabled and retains the
accumulated value when enable is false.  This timer 
type is reset only by making the reset input true.
* current version consumes 96 bytes RAM
------------------------------------------------------------*/

RetentiveTimer::RetentiveTimer(unsigned long pre)
  : Multi_Timer(pre){};

RetentiveTimer::~RetentiveTimer() {}  // give a destructor

// Establish reset conditions for retentive ON delay timer
bool RetentiveTimer::reset() {
  return (_Reset);
}  // End of Retentive timer

/*==============================================================

              Pulse Generator Timer Definition
-----------------------------------------------------------
A timer which runs when enabled and not reset. Resets
itself upon reaching preset then restarts the timing cycle
automatically as long as enable is true. The pulse output
is the _Done_OSF/OSR from the base class.
* current version consumes 60 bytes RAM
------------------------------------------------------------
*/
PulseGenTimer::PulseGenTimer(unsigned long pre)
  : Multi_Timer(pre){};

PulseGenTimer::~PulseGenTimer() {} // give a destructor

// Establish reset conditions for pulse generator timer
bool PulseGenTimer::reset() {
  //return (_Reset or _Done or !_Enable);
  return(_Reset or _Done_OSR);
}  //End of class PulseGenTimer

/*============================================================

         Latched /Retentive/ timer class definition
--------------------------------------------------------------
A timer which starts when 'setCtrl' is called with a logic true
and if reset is false. Once started, timer continues, even
if _Enable subsequently goes false, until preset is reached.
This timer type is reset only by making the reset input true.
* current version consumes 92 bytes RAM
--------------------------------------------------------------*/

LatchedTimer::LatchedTimer(unsigned long pre)
  : Multi_Timer(pre){};

LatchedTimer::~LatchedTimer() {}  // give a destructor
/*
Caller starts Latched timer with a call to Start with strt
true. Once started the timer runs independently to preset.
_Control is used to isolate the latched timer from '_Enable'
in the base class which would override the latch. Because of
this, a Latched Timer has no need of _Enable at all.
*/
void LatchedTimer::Start(bool strt) {
  if (!strt) return;
  _Control = true;
}
// Establish reset conditions for self-latching ON delay timer
bool LatchedTimer::reset() {
  return (_Reset and _Done);
}  // End of class LatchedTimer

/*=================================================================

                   Retriggerable Monostable timer
---------------------------------------------------------------
Runs when enable is true. A change of state on the 'control'
input resets the timer and restarts the timing cycle if enable
is true. Continuous cycling of the control input at a rate
faster than the time delay will cause the done status flag
to remain low indefinitely.
----------------------------------------------------------------
*/

RetriggerableTimer::RetriggerableTimer(unsigned long pre)
  : Multi_Timer(pre){ };

RetriggerableTimer::~RetriggerableTimer() {}  // give a destructor

bool RetriggerableTimer::reset() {
  /*
    Generate a reset whenever control input changes
    or _Reset is true.
  */
  _WD_Rising_OS = (_Control and _WD_Rising_Setup);
  _WD_Rising_Setup = !_Control;
  _WD_Falling_OS = (!_Control and _WD_Falling_Setup);
  _WD_Falling_Setup = _Control;
  return (_WD_Falling_OS or _WD_Rising_OS or _Reset);

}  // End of Retriggerable timer

/*=================================================================

                 FlasherTimer Timer class definition
-----------------------------------------------------------------
This timer runs and resets itself automatically when enabled.
It is basically an enhanced OnDelay timer. The second constructor
argument specifies an ON time for a special output (_FlashOut)
unique to this type. If _Enable goes false the timer is reset
immediately.

 Set onTime to some fraction of pre
*/
FlasherTimer::FlasherTimer(unsigned long pre, unsigned long onTime)
  : Multi_Timer(pre, onTime) {}

FlasherTimer::~FlasherTimer() {}  // give a destructor

bool FlasherTimer::reset() {
  //return (!_Enable or _Done);
return(!_Enable or _Done_OSR);
}

bool FlasherTimer::isFlashing() {

  // 4/2/24 : Moved _FlashOut code from update() to isFlashing()
  // so that _FlashOut only applies to FlasherTimer objects.
  //
  // Code below will turn on a common cathode LED for _OnTime
  // milliseconds when timing cycle starts.

  if (_Accumulator <= _OnTime) {
    _FlashOut = true;
  } else _FlashOut = false;

  return (_FlashOut and _Enable);
}

// 11/17/18 : Added method to runtime adjust _OnTime
// 12/18/18 : Added forced reset

void FlasherTimer::setOnTime(unsigned long newOnTime) {
  _OnTime = newOnTime;
  _Accumulator = _Preset;  // Force a reset when new onTime loaded
}
// End of FlasherTimer timer
