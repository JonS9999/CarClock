#ifndef _MY_DEBUG_H_
#define _MY_DEBUG_H_

//
//  System include files :
//
#include <Arduino.h>

// Function to print time at the top of the minute :
extern void MyDebug_MinuteReport ( bool ForceUpdate );

// Function to simulate an "AP button" press after a specific delay :
extern bool MyDebug_IsApButtonPressed(void);

// Function to check the serial port for debugging commands :
extern void MyDebug_SerialCheck(void);

#endif // !_MY_DEBUG_H_
