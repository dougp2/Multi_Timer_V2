/* filename: Multi_Timer_V2.h  6/18/24

 https://forum.arduino.cc/t/renamed-iterating-through-a-series-of-objects-original-title-oop-this-pointer-questions/1244434

The thread noted above holds progression of original Multi_timer to
this version which incorporates a method to update all timers with
only one call by the user. Original required user to call update()
for each timer object instantiated.

If only one timer is instantiated you can still use the original
update() and save some flash and clock cycles.

Some basic timers with a (it is hoped) simple interface.
*/

#ifndef MULTI_TIMER_V2_H
#define MULTI_TIMER_V2_H

#include "Arduino.h"

class Multi_Timer {

private:
  // Variables for setting up list of objects (timers) for use by
  // Update_All_Timers().
  //
  static Multi_Timer *first;  // set location of first base class
  Multi_Timer *next;          // = nullptr;

public:
  Multi_Timer(unsigned long);                 // constructor declaration
  Multi_Timer(unsigned long, unsigned long);  // flasher constructor
  ~Multi_Timer();                             // destructor declaration

  // Enable/disable a timer
  void setEnable(bool);

  // Reset or unreset a timer
  void setReset(bool);

  // Find out if a timer is enabled
  bool isEnabled() const;

  // Find out if a timer is reset
  bool isReset() const;

  // Start signal for certain timers
  void setCtrl(bool);

  // Find out if a timer has reached preset value
  bool isDone() const;

  // Find out if a timer is progressing toward preset
  bool isRunning() const;

  // Returns state of timer done rising one-shot
  bool getDoneRose() const;

  // Returns state of timer done falling one-shot
  bool getDoneFell() const;

  // Find out the current accumulator of a timer
  unsigned long getCount() const;

  unsigned long pre;


protected:
  virtual bool reset() = 0;

public:
  /* ========================================================

   The 'update' function is the heart of the thing.
   ----------------------------------------------------
   Updates timer accumulated value & conditions flags '_Done',
  '_Done_Rising_OS', '_Done_Falling_OS' and '_TimerRunning'.
  
   Returns boolean status of _Done.  update() must be called
   periodically in loop to update the flags and accumulated
   value for each timer.
   ====================================================*/

  virtual boolean update();  // Updates the timer which calls it.

  /* =============================================================
              Update All Timers
   ---------------------------------------------------------------
   Loop through all timer objects and update them by successively
   calling update(). This relieves the user of adding a call to 
   xxx.update() for every timer instance.
   ----------------------------------------------------------------*/

  static void updateAllTimers();

protected:

  bool _Reset : 1;
  bool _Enable : 1;
  bool _Done : 1;
  bool _Control : 1;
  bool _TimerRunning : 1;
  bool _Done_OSF : 1;
  bool _Done_OSR : 1;
  bool _Done_Rising_Setup : 1;
  bool _Done_Falling_Setup : 1;
  bool _FlashOut;

  unsigned long _Accumulator;
  unsigned long _CurrentMillis;
  unsigned long _LastMillis;
  unsigned long _OnTime;
  unsigned long _Preset;



};  //end of base class Multi_Timer declarations

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Define various types of timers which inherit/derive from Multi_Timer.
   The timers differ in functionality mainly by their reset methods.
   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
//                  On Delay Timer class
//--------------------------------------------------------------------
// A timer which runs when reset is false and enable is true
// If reset is true or enable is false it is reset.
//--------------------------------------------------------------------

class OnDelayTimer : public Multi_Timer {
public:
  OnDelayTimer(unsigned long);  // constructor
  ~OnDelayTimer();              // destructor

  virtual bool reset();
};  // End of class OnDelayTimer

/*===================================================================

     Off delay timer - this timer redefines update()
 
Off delay timer functions like an on delay timer but enable and done
operate inversely to an on delay timer. Timer is reset and done is
true when enable is true.  Timing cycle begins when enable goes false
and after the delay time done goes false. 
//------------------------------------------------------------------
*/

class OffDelayTimer : public Multi_Timer {

public:
  OffDelayTimer(unsigned long);  //constructor
  ~OffDelayTimer();

  virtual bool update();

  virtual bool reset();
};  // End of class OffDelayTimer

/*==============================================================

             Retentive timer class definition
--------------------------------------------------------------
A timer which accumulates when enabled. Accumulated value is
retained when enable is false.  This timer type is reset only
by making the reset input true.
* current version consumes 96 bytes RAM
--------------------------------------------------------------*/

class RetentiveTimer : public Multi_Timer {
public:
  RetentiveTimer(unsigned long);

  ~RetentiveTimer();

  // Establish reset conditions for retentive ON delay timer
  virtual bool reset();
};  // End of class retentive timer

/*==============================================================

              Pulse Generator Timer
-----------------------------------------------------------
A timer which runs when enabled and not reset. Resets
itself upon reaching preset then restarts the timing cycle
automatically as long as enable is true.
* current version consumes 60 bytes RAM
------------------------------------------------------------*/

class PulseGenTimer : public Multi_Timer {
public:
  PulseGenTimer(unsigned long);

  ~PulseGenTimer();

  // Establish reset conditions for pulse generator timer
  virtual bool reset();
};  //End of class PulseGenTimer
//-------------------------------------------

/*============================================================

         Latched /Retentive/ timer class definition

--------------------------------------------------------------
Caller starts Latched timer with a call to Start with strt
true. Once started the timer runs independently to preset.
_Control is used to isolate the latched timer from '_Enable'
in the base class which would override the latch. Because of
this, a Latched Timer has no need of _Enable at all.
* current version consumes 92 bytes RAM
--------------------------------------------------------------*/

class LatchedTimer : public Multi_Timer {
public:
  LatchedTimer(unsigned long);
  ~LatchedTimer();
  // Caller starts Latched timer here with a pulse signal.
  void Start(bool);

  // Establish reset conditions for self-latching ON delay timer
  virtual bool reset();
};
// End of class LatchedTimer
//-----------------------------------------------------

/*=================================================================

                   Retriggerable timer
---------------------------------------------------------------
>> Runs when enable is true. If enable is false timer will <<
>> function as an onDelay timer.                           <<

Whhen enabled a change of state on the 'control' input resets
the timer and restarts the timing cycle. Continuous cycling
of the control input at a rate faster than the time delay will
cause the done status flag to remain low indefinitely.
--------------------------------------------------------------*/

class RetriggerableTimer : public Multi_Timer {
public:
  RetriggerableTimer(unsigned long);
  ~RetriggerableTimer();

  virtual bool reset();

private:
  bool _WD_Rising_OS : 1;
  bool _WD_Falling_OS : 1;
  bool _WD_Falling_Setup : 1;
  bool _WD_Rising_Setup : 1;
};
// End of class RetriggerableTimer

/*
                 Flasher Timer class definition
-----------------------------------------------------------------
This timer runs and resets itself automatically when enabled.
It is basically an enhanced pulse generator timer. The second
constructor argument specifies an ON time for a special output
(_FlashOut) unique to this type. If _Enable goes false the
timer is reset immediately.

Flasher timer can be used as a pulse generator timer but the 
flashout and settable on time features will incur a penalty
in memory and clock cycle usage.

onTime must be some fraction of pre
--------------------------------------------------------------------*/

class FlasherTimer : public Multi_Timer {
public:
  FlasherTimer(unsigned long, unsigned long);
  ~FlasherTimer();

  virtual bool reset();

  // 4/2/24 : Moved _FlashOut decision from update() to getFlash()
  // so that _FlashOut code only applies to FlasherTimer objects.
  //

  bool isFlashing();

  // 11/17/18 : Added method to runtime adjust _OnTime
  // 12/18/18 : Added forced reset when _Ontime changed

  void setOnTime(unsigned long);
};
// end of class FlasherTimer
//---------------------------------------------

#endif
